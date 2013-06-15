#include <iostream>
#include "Parser.h"
#include "Defines.h"
#include "FileManager.h"
#include "ByteBuffer.h"
#include "Termino.h"
#include "Utilidades.h"
#include <vector>
using namespace std;

// FALTA PONER ../ EN CADA UNO
string dir_repositorios = "Repositorios";
string file_repositorios = "Repositorios/repos.txt";
string file_paths = "/paths.txt";
string file_off = "/off.txt";

void indexar(string nombre, string dir) {

	//TODO: almacenamiento de repositorio

	//Crea carpeta para repositorio
	string repo_dir = dir_repositorios+"/"+nombre;
	mkdir(repo_dir.c_str() , S_IRWXU | S_IRWXG | S_IRWXO);

	//Agrega al archivo de repos
	ofstream repos;
	repos.open(file_repositorios.c_str(),ios::app | ios::binary);

	//Formato: num de bytes (short) + nombre (variable)
	short numBytes = nombre.length();
	repos.write((char*)&numBytes,sizeof(numBytes));
	repos << nombre;
	repos.close();

	//Creo archivo de documentos
	ofstream paths((repo_dir+file_paths).c_str(),ios::app | ios::binary);
	ofstream offsets((repo_dir+file_off).c_str(),ios::app | ios::binary);

	//Agrego el header de paths: numBytes + path raiz
	short numBytesRaiz = dir.length();
	paths.write((char*)&numBytesRaiz,sizeof(numBytesRaiz));
	paths << dir;

	Parser* parser = Parser::getInstance();
	parser->parsearDirectorio(dir,repo_dir,paths,offsets);

	paths.close();
	offsets.close();



}

// Devuelve el termino de la lista de terminos completos dado el registro en el que se encuentra
string obtenerTermino(ifstream& tIdxIn, string listaTerminosCompletos, int registro) {

	short cantCaracteres;
	int tOff;
	//Se posiciona en el registro
	tIdxIn.seekg(registro*TAM_TERMINO_REG,tIdxIn.beg);
	tIdxIn.read((char*)&cantCaracteres,2);
	tIdxIn.read((char*)&tOff,4);

	string palabra = listaTerminosCompletos.substr(tOff,cantCaracteres);

	return palabra;


}

// Devuelve la posicion en el archivo de terminos completos donde puede llegar a estar la palabra buscada
int busquedaBinariaTerminos(string query, int cantRegistros, ifstream& tIdxIn, string listaTerminosCompletos) {
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

int obtenerOffsetDocsTerminosCompletos(ifstream& tIdxIn, int registro) {
	tIdxIn.seekg(registro*TAM_TERMINO_REG+10,tIdxIn.beg); // +10 porque saltea campos anteriores hasta docs
	int offset;
	tIdxIn.read((char*)&offset,sizeof(offset));
	return offset;
}


//Devuelve el offset a documentos de la query o -1 si no lo encuentra. Setea el offset al siguiente pasado por parametro
int busquedaEnBloque(string query, int pos, ifstream& tIdxIn, string listaTerminosCompletos, ifstream& tLexico, int* docOffsetSiguiente) {


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
		cout << "El termino " + query + " esta en la lista de completos" << endl;
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
				cout << "El termino " + query + " esta en el archivo de lexico" << endl;
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



string obtenerDocs(ifstream& tDocs, int docOffset, int docOffsetSiguiente) {
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

Termino* decodeDocRegister(string palabra, string docs) {
	//LO HAGO A MANO ACA, DESPUES VER COMO ADAPTARLO A LA CLASE CODER
	int tam = 0;
	int cantDocs = Coder::decode(docs,&tam);
	docs = docs.substr(tam,docs.length()-tam);
	cout << "Cantidad de documentos: " << cantDocs << endl;
	cout << "Documentos: " << endl;
	int doc = 0;
	Termino* termino = new Termino(palabra);
	int* listaDocumentos = (int*)malloc(cantDocs*sizeof(int));
	// Leo documentos
	for(int i = 0; i < cantDocs; i++) {
		doc+= Coder::decode(docs,&tam);
		cout << doc << endl;
		listaDocumentos[i] = doc;
		docs = docs.substr(tam,docs.length()-tam);
	}
	// Leo posiciones
	int cantPos;
	int pos;
	for(int j = 0; j < cantDocs; j++) {
		cantPos = Coder::decode(docs,&tam);
		docs = docs.substr(tam,docs.length()-tam);
		cout << "Cantidad de posiciones: " << cantPos << endl;
		cout << "Posiciones:" << endl;
		pos = 0;
		// VA DELETE?????
		vector<long>* listaPosiciones = new vector<long>;
		for(int k = 0; k < cantPos; k++) {
			pos+= Coder::decode(docs,&tam);
			//cout << pos << endl;
			listaPosiciones->push_back(pos);
			docs = docs.substr(tam,docs.length()-tam);
		}
		termino->addPositionsForDoc(listaDocumentos[j],listaPosiciones);
	}
	free(listaDocumentos);
	return termino;
}

void intersecar(vector<int>* interseccion, vector<int> docsAIntersecar) {
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

void intersecarPosiciones(vector<long>* interseccion, vector<long> posicionesAIntersecar) {
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

void consulta(string repo, string query) {
	// Abro todos los archivos
	ifstream tIdxIn(("Repositorios/" + repo + "/terminosIdx").c_str(),ios::binary|ios::in);
	ifstream tListaIn(("Repositorios/" + repo + "/terminosLista").c_str(),ios::binary|ios::in);
	ifstream tLexico(("Repositorios/" + repo + "/lexico").c_str(),ios::binary|ios::in);
	ifstream tDocs(("Repositorios/" + repo + "/documentos").c_str(),ios::binary|ios::in);

	// Calculo la cantidad total de registros
	tIdxIn.seekg(0,tIdxIn.end);
	int cantRegistros = tIdxIn.tellg()/TAM_TERMINO_REG;
	tIdxIn.seekg(0,tIdxIn.beg);

	vector<Termino*> terminos;

	//Me traigo la lista entera de terminos completos para busqueda binaria

	tListaIn.seekg (0, tListaIn.end);
	int length = tListaIn.tellg();
	tListaIn.seekg (0, tListaIn.beg);

	char * bufferListaTerminosCompletos = new char [length];

	tListaIn.read (bufferListaTerminosCompletos,length);

	string listaTerminosCompletos = bufferListaTerminosCompletos;

	char* token = strtok((char*) query.c_str(), kSEPARADORES);
	if (token != NULL)
		Utilidades::string_a_minusculas(token);
	int posicion = 0;
	bool falla = false;
	while (token != NULL && !falla) {
		if (!Utilidades::isNumber(token) && strlen(token) > 1 && !Parser::getInstance()->isStopWord(token)) {//TODO: ESTA FEO PIDIENDOSELO AL PARSER: CAMBIAR

			int pos = busquedaBinariaTerminos(token,cantRegistros,tIdxIn,listaTerminosCompletos);

			//Tengo que traerme el registro de documentos
			int docOffsetSiguiente;
			int docOffset = busquedaEnBloque(token,pos, tIdxIn, listaTerminosCompletos, tLexico, &docOffsetSiguiente);

			string docRegister;
			if(docOffset >= 0){
				//Caso especial: que sea el ultimo
				if(docOffsetSiguiente < 0){
					tDocs.seekg (0, tDocs.end);
					docOffsetSiguiente = tDocs.tellg()*8;//Final del archivo de documentos
					tDocs.seekg (0, tDocs.beg);
				}
				docRegister = obtenerDocs(tDocs,docOffset,docOffsetSiguiente);
				terminos.push_back(decodeDocRegister(token, docRegister));
			} else {
				cout << "Palabra: " << token << endl;
				cout << "Consulta no encontrada" << endl;
				falla = true;
			}
		} else {
			terminos.push_back(NULL); //se salteo el termino
		}
		//Tomo el siguiente token
		token = strtok(NULL, kSEPARADORES);
		if (token != NULL)
			Utilidades::string_a_minusculas(token);
		posicion++;
	}

	if(!falla) {
		//Normalizacion de posiciones e interseccion de documentos
		vector<int> interseccionDocs;
		vector<Termino*>::const_iterator it;
		int counter = 0;
		for (it = terminos.begin(); it != terminos.end(); ++it) {
			if((*it)!=NULL) {

				if(interseccionDocs.empty()) {
					interseccionDocs = (*it)->docs;
				} else {
					intersecar(&interseccionDocs,(*it)->docs);
				}

				vector<long>* posicionesNormalizadas = new vector<long>;
				std::vector<vector<long> >::const_iterator positionListIt;
				for (positionListIt = (*it)->listaPosiciones.begin(); positionListIt != (*it)->listaPosiciones.end(); ++positionListIt){
					std::vector<long>::const_iterator positionIt;
					for(positionIt = positionListIt->begin(); positionIt != positionListIt->end() ; ++positionIt){
						posicionesNormalizadas->push_back((*positionIt)-counter);
					}
					(*it)->listaPosicionesNormalizadas.push_back(*posicionesNormalizadas);
				}
			}
			counter++;
		}


		vector<int>::const_iterator itInterseccion;
		vector<Termino*>::const_iterator itTerminos;
		bool salir;
		int position;
		vector<long> posicionesIntersecar;
		for(itInterseccion = interseccionDocs.begin(); itInterseccion != interseccionDocs.end(); ++itInterseccion) {
			cout << endl;
			cout << "Posibles archivos: " << (*itInterseccion) << endl;
			bool noHayMatch = false;
			for (itTerminos = terminos.begin(); itTerminos != terminos.end() && !noHayMatch; ++itTerminos) {
				if((*itTerminos)!=NULL) {
					cout << (*itTerminos)->palabra << endl;
					vector<int>::const_iterator itDoc;
					position = 0;
					salir = false;
					// Recorro documentos
					for(itDoc = (*itTerminos)->docs.begin(); itDoc!=(*itTerminos)->docs.end() && !salir; ++itDoc) {
						if(*itDoc == *itInterseccion) {
							salir = true;
						} else {
							position++;
						}
					}
					if(posicionesIntersecar.empty()) {
						posicionesIntersecar = (*itTerminos)->listaPosicionesNormalizadas[position];
					} else {
						intersecarPosiciones(&posicionesIntersecar,(*itTerminos)->listaPosicionesNormalizadas[position]);
						if(posicionesIntersecar.empty()) {
							noHayMatch = true;
						}
					}
				}
			}
			if(!posicionesIntersecar.empty()) {
				cout << "Match en el documento " << *itInterseccion << endl;
			} else {
				cout << "No hay match" << endl;
			}
		}
	}
	delete[] bufferListaTerminosCompletos;
	tIdxIn.close();
	tListaIn.close();
	tLexico.close();
	tDocs.close();

}

int main(int argc, char** argv) {

	/*if (argc < 2) {
		cout << "Pocos parámetros" << endl;
		return 1;
	}
	string instruccion = argv[1];

	if (instruccion == "i") {
		// Indexa
		if (argc != 4) {
			cout << "No se puede indexar" << endl;
			return 1;
		}*/
	//DESCOMENTAR ESTO Y COMENTAR EL OTRO
	//	indexar(argv[2], argv[3]);
//	indexar("probando","prueba");
	//OVERFLOW DE POSICIONES!!!!!!!!
/*
	} else if (instruccion == "q") {
		// Consulta
		int c;
		char* r = NULL;
		char* q = NULL;

		while ((c = getopt(argc, argv, "r:q:")) != -1) {
			switch (c) {
			case 'r':
				r = optarg;
				break;
			case 'q':
				q = optarg;
				break;
			case '?':
				cout << "Instrucción inválida" << endl;
				return 1;
			default:
	(			return 1;
			}
		}
*/
		//DESCOMENTAR ESTO Y COMENTAR EL OTRO
		//consulta(r,q);
		consulta("probando","hector");
//
//	} else {
//		cout << "Instrucción inválida" << endl;
//		return 1;
//	}

	return 0;
}

