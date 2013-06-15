/*
 * IndexManager.cpp
 *
 *  Created on: 24/05/2013
 *      Author: lara
 */

#include "IndexManager.h"
#include "FileManager.h"
#include "Defines.h"
#include "ByteBuffer.h"
#include <sstream>
#include <fstream>
#include <iostream>
#include <stdlib.h>




IndexManager* IndexManager::instance = 0;


IndexManager::IndexManager() {
	this->docOffset = 0;
	this->termOffset = 0;
	this->lexicOffset = 0;
	this->cantPalabras = 0;
	this->lastTerminoCompleto = "";

}

IndexManager::~IndexManager() {
	// TODO Auto-generated destructor stub
}

IndexManager* IndexManager::getInstance() {
	if (instance == 0) {
		instance = new IndexManager();
	}
	return instance;
}

string IndexManager::toString(int number) {
	stringstream ss; //create a stringstream
	ss << number; //add number to the stream
	return ss.str(); //return a string with the contents of the stream
}
// Devuelve la posicion en el archivo de terminos completos donde puede llegar a estar la palabra buscada
int IndexManager::busquedaBinariaTerminos(string query, int cantRegistros, ifstream& tIdxIn, string listaTerminosCompletos) {
	int der = cantRegistros - 1;
	int izq = 0;
	int medio;
	int res;
	bool encontrado = false;
	string palabra;
	while(izq <= der && !encontrado) {
		medio = (izq + der)/2;
		palabra = obtenerTermino(tIdxIn,listaTerminosCompletos,medio);
		res = query.compare(palabra);
		// Encontro el termino
		if(res == 0) {
			encontrado = true;
		// Esta mas adelante
		} else if( res > 0) {
			izq = medio +1;
		//Esta mas atras
		} else {
			der = medio - 1;
		}
	}

	//Arreglo al metodo de busqueda binaria para que me devuelva el bloque en el que esta
	//Si no lo encuentra en el de terminos completos pero el ultimo resultado me dio mayor, devuelvo el anterior
	if(res < 0) {
		return medio - 1;
	}
	return medio;
}

int IndexManager::obtenerOffsetDocsTerminosCompletos(ifstream& tIdxIn, int registro) {
	tIdxIn.seekg(registro*TAM_TERMINO_REG+10,tIdxIn.beg); // +10 porque saltea campos anteriores hasta docs
	int offset;
	tIdxIn.read((char*)&offset,sizeof(offset));
	return offset;
}


//Devuelve el offset a documentos de la query o -1 si no lo encuentra. Setea el offset al siguiente pasado por parametro
int IndexManager::busquedaEnBloque(string query, int pos, ifstream& tIdxIn, string listaTerminosCompletos, ifstream& tLexico, int* docOffsetSiguiente) {


	string terminoCompleto = obtenerTermino(tIdxIn,listaTerminosCompletos,pos);
	int offLexico, offDocs;
	int counter = 0;
	bool encontrado = false;
	unsigned short iguales, distintos;

	string palabra;

	tIdxIn.seekg(pos*TAM_TERMINO_REG+6,tIdxIn.beg); // +6 porque me salteo los dos primeros campos
	tIdxIn.read((char*)&offLexico,sizeof(offLexico));
	tLexico.seekg(offLexico,tLexico.beg);

	//PRIMERO ME FIJO SI ES EL DE TERMINOS COMPLETOS
	if(terminoCompleto.compare(query) == 0) {
//		cout << "El termino " + query + " esta en la lista de completos" << endl;
		offDocs = obtenerOffsetDocsTerminosCompletos(tIdxIn,pos);
		encontrado = true;

		//OBTENGO EL OFFSET SIGUIENTE
		tLexico.read((char*)&iguales,sizeof(iguales));
		tLexico.read((char*)&distintos,sizeof(distintos));

		char* caracteresDistintos = new char[distintos];
		tLexico.read(caracteresDistintos,distintos);
		tLexico.read((char*)docOffsetSiguiente,sizeof(*docOffsetSiguiente));

	// BUSCO EN EL BLOQUE EN EL ARCHIVO DE LEXICO
	} else {
		while(counter < kNFRONTCODING && !encontrado) {
			tLexico.read((char*)&iguales,sizeof(iguales));
			tLexico.read((char*)&distintos,sizeof(distintos));

			char* caracteresDistintosAux = new char[distintos];
			tLexico.read(caracteresDistintosAux,distintos);
			tLexico.read((char*)&offDocs,sizeof(offDocs));
			string caracteresDistintos = caracteresDistintosAux;
			caracteresDistintos = caracteresDistintos.substr(0,distintos);

			// Formo la palabra con los caracteres iguales del termino completo y los distintos extraidos del archivo de lexico
			palabra = terminoCompleto.substr(0,iguales) + caracteresDistintos;
		//	cout << palabra << endl;

			counter++;

			if(palabra.compare(query) == 0) {
			//	cout << "El termino " + query + " esta en el archivo de lexico" << endl;
				encontrado = true;
				if(counter == kNFRONTCODING -1) {
					*docOffsetSiguiente = obtenerOffsetDocsTerminosCompletos(tIdxIn,pos+1);
				}else{
					tLexico.read((char*)&iguales,sizeof(iguales));
					tLexico.read((char*)&distintos,sizeof(distintos));

					char* caracteresDistintos = new char[distintos];
					tLexico.read(caracteresDistintos,distintos);
					tLexico.read((char*)docOffsetSiguiente,sizeof(*docOffsetSiguiente));
					//Caso especial: que sea el ultimo
					if(tLexico.eof()){
						 *docOffsetSiguiente = -1;
					}
				}
			}

		}
	}

	if(!encontrado) offDocs = -1;
	return offDocs;

}



string IndexManager::obtenerDocs(ifstream& tDocs, int docOffset, int docOffsetSiguiente) {
	int fileOffset = docOffset/8;
	tDocs.seekg(fileOffset,tDocs.beg);// Lo posiciona en el byte correspondiente al bit

	unsigned char byteLeidoAux;
	string byteLeido = "";
	string docs = "";
	//Calculo cuantos bytes tengo que leer
	int endOffset = docOffsetSiguiente/8;
	if(docOffsetSiguiente%8!=0) {
		endOffset++;
	}
	int len;
	for(int i = 0; i < endOffset-fileOffset; i++) {
		tDocs.read((char*)&byteLeidoAux,1);
		// Lo convierto a tira de bits
		byteLeido = Coder::numberToBinary((int)byteLeidoAux);
		//Agrego ceros
		len = byteLeido.length();
		for(int j = 0; j < 8-len; j++) {
			byteLeido = "0" + byteLeido;
		}
		docs+=byteLeido;
		byteLeido.clear();
	}

	return docs.substr(docOffset - fileOffset*8,docOffsetSiguiente-docOffset);


}

Termino* IndexManager::decodeDocRegister(string palabra, string docs) {
	//LO HAGO A MANO ACA, DESPUES VER COMO ADAPTARLO A LA CLASE CODER
	int tam = 0;
	int cantDocs = Coder::decode(docs,&tam);
	docs = docs.substr(tam,docs.length()-tam);
//	cout << "Cantidad de documentos: " << cantDocs << endl;
//	cout << "Documentos: " << endl;
	int doc = 0;
	Termino* termino = new Termino(palabra);
	int* listaDocumentos = (int*)malloc(cantDocs*sizeof(int));
	// Leo documentos
	for(int i = 0; i < cantDocs; i++) {
		doc+= Coder::decode(docs,&tam);
//		cout << doc << endl;
		listaDocumentos[i] = doc;
		docs = docs.substr(tam,docs.length()-tam);
	}
	// Leo posiciones
	int cantPos;
	int pos;
	for(int j = 0; j < cantDocs; j++) {
		cantPos = Coder::decode(docs,&tam);
		docs = docs.substr(tam,docs.length()-tam);
//		cout << "Cantidad de posiciones: " << cantPos << endl;
//		cout << "Posiciones:" << endl;
		pos = 0;
		// VA DELETE?????
		vector<long>* listaPosiciones = new vector<long>;
		for(int k = 0; k < cantPos; k++) {
			pos+= Coder::decode(docs,&tam);
//			cout << pos << endl;
			listaPosiciones->push_back(pos);
			docs = docs.substr(tam,docs.length()-tam);
		}
		termino->addPositionsForDoc(listaDocumentos[j],listaPosiciones);
	}
	free(listaDocumentos);
	return termino;
}

void IndexManager::intersecar(vector<int>* interseccion, vector<int> docsAIntersecar) {
	std::vector<int>::const_iterator interseccionIt = interseccion->begin();
	std::vector<int>::const_iterator docsIt = docsAIntersecar.begin();
	vector<int> nuevaInterseccion;
	while(interseccionIt != interseccion->end() && docsIt != docsAIntersecar.end()) {
		if(*interseccionIt < *docsIt) {
			++interseccionIt;
		} else if(*interseccionIt > * docsIt){
			++docsIt;
		} else {
			nuevaInterseccion.push_back(*interseccionIt);
			++interseccionIt;
			++docsIt;
		}
	}
	*interseccion = nuevaInterseccion;
}

void IndexManager::intersecarPosiciones(vector<long>* interseccion, vector<long> posicionesAIntersecar) {
	std::vector<long>::const_iterator interseccionIt = interseccion->begin();
	std::vector<long>::const_iterator docsIt = posicionesAIntersecar.begin();
	vector<long> nuevaInterseccion;
	while(interseccionIt != interseccion->end() && docsIt != posicionesAIntersecar.end()) {
		if(*interseccionIt < *docsIt) {
			++interseccionIt;
		} else if(*interseccionIt > * docsIt){
			++docsIt;
		} else {
			nuevaInterseccion.push_back(*interseccionIt);
			++interseccionIt;
			++docsIt;
		}
	}
	*interseccion = nuevaInterseccion;
}

// Devuelve el termino de la lista de terminos completos dado el registro en el que se encuentra
string IndexManager::obtenerTermino(ifstream& tIdxIn, string listaTerminosCompletos, int registro) {

	short cantCaracteres;
	int tOff;
	//Se posiciona en el registro
	tIdxIn.seekg(registro*TAM_TERMINO_REG,tIdxIn.beg);
	tIdxIn.read((char*)&cantCaracteres,2);
	tIdxIn.read((char*)&tOff,4);

	string palabra = listaTerminosCompletos.substr(tOff,cantCaracteres);

	return palabra;

}

void IndexManager::saveTerminoCompleto(Termino* termino,ofstream& tIdx, ofstream& tLista) {

	short size;
	size = termino->palabra.size(); // Transformo el size a short para que ocupe 2 bytes

	FileManager::getInstance()->saveToFile(&size,sizeof(size),tIdx);
	FileManager::getInstance()->saveToFile(&termOffset,sizeof(termOffset),tIdx);
	FileManager::getInstance()->saveToFile(&lexicOffset,sizeof(lexicOffset),tIdx);
	FileManager::getInstance()->saveToFile(&docOffset,sizeof(docOffset),tIdx);

	FileManager::getInstance()->saveToFile(termino->palabra,tLista);

	this->lastTerminoCompleto = termino->palabra;

}

void IndexManager::saveLexico(Termino* termino, ofstream& tLexico) {
	bool distintos = false;
	unsigned short iguales = 0;
	while(!distintos && (iguales <= termino->palabra.size() || iguales<=lastTerminoCompleto.size())) {
		if(termino->palabra[iguales] != lastTerminoCompleto[iguales]) {
			distintos = true;
		} else {
			iguales++;
		}
	}

	unsigned short cantCaractDistintos = termino->palabra.size() - iguales;

	FileManager::getInstance()->saveToFile(&iguales,sizeof(iguales),tLexico);
	FileManager::getInstance()->saveToFile(&cantCaractDistintos,sizeof(cantCaractDistintos),tLexico);
	FileManager::getInstance()->saveToFile(&(termino->palabra[iguales]),tLexico);
	FileManager::getInstance()->saveToFile(&docOffset,sizeof(docOffset),tLexico);


	/* Actualizacion del offset de lexico
	 * CADA REGISTRO DE LEXICO TIENE:
	 * 		- 2 BYTES PARA REPETIDOS
	 * 		- 2 BYTES PARA DISTINTOS
	 * 		- CARACTERES DISTINTOS (VARIABLE)
	 * 		- 4 BYTES PARA OFFSET A ENTEROS
	 */
	this->lexicOffset+=2+2+cantCaractDistintos+4;
}

void IndexManager::indexTerm(Termino* termino, ofstream& tIdx,ofstream& tLista,ofstream& tLexico,ofstream& tDocs) {
	//Pasar docs y posiciones a distancias



	termino->convertIntoDistances();

	//Guardar el termino en disco
	string docRegister = "";

	docRegister+=termino->distDocs;

	std::vector<string>::const_iterator positionIt;
	for(positionIt = termino->distPositions.begin();positionIt != termino->distPositions.end(); ++positionIt ) {
		docRegister+=*positionIt;
	}

	//cout << termino->palabra + ": "+ docRegister << endl;
	// Guardo registro en el archivo de documentos
	ByteBuffer::getInstance()->saveBytes(docRegister,tDocs);

	//Guardo termino en el archivo de terminos completos o en el de lexico

	if(cantPalabras % kNFRONTCODING == 0) {
		saveTerminoCompleto(termino,tIdx, tLista);
		//El offset de termino se actualiza cada vez que cambio el termino completo
		this->termOffset+=termino->palabra.size();
	} else {
		//El offset de lexico se actualiza dentro de saveLexico ya que necesito la cantidad de caracteres distintos
		saveLexico(termino,tLexico);
	}


	this->cantPalabras++;
	// El offset de documentos se actualiza siempre
	this->docOffset+=docRegister.size();

}

void IndexManager::reset() {
	this->docOffset = 0;
	this->termOffset = 0;
	this->lexicOffset = 0;
	this->cantPalabras = 0;
	this->lastTerminoCompleto = "";
}
