/*
 * Parser.cpp
 *
 *  Created on: 01/05/2013
 *      Author: lara
 */

#include "Parser.h"
#include <sstream>
#include <algorithm>
#include "IndexManager.h"
#include "ByteBuffer.h"
#include "Defines.h"
#include "Utilidades.h"

int MAX_MEM = 1024;

Parser* Parser::instance = 0;

Parser::Parser() {

}

Parser* Parser::getInstance() {
	if (instance == 0) {
		instance = new Parser();
	}
	return instance;
}

// Devuelve la posicion
int busquedaBinaria(string texto, vector<TerminoRegister>* terminos) {
	int der = terminos->size() - 1;
	int izq = 0;
	int medio = -1;
	int res;
	bool encontrado = false;
	string palabra;
	while(izq <= der && !encontrado) {
		medio = (izq + der)/2;
		palabra = terminos->at(medio).getTermino();
		res = texto.compare(palabra);
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

	if(!encontrado) medio = -1;
	return medio;
}

/*
 * Recorre el archivo y va separando en tokens cada linea. Asigna los terminos al vector de terminos
 */
void Parser::processFile(const char* path, short nro_doc,
		vector<TerminoRegister>* terminos, int* memoriaUsada) {
	string line;
	ifstream fin;
	fin.open(path);
	int posicion = 1;
	while (!fin.eof()) {

		getline(fin, line);
		char* token = strtok((char*) line.c_str(), kSEPARADORES);
		if (token != NULL)
			Utilidades::string_a_minusculas(token);
		bool nuevo = true;
		int pos;
		while (token != NULL) {
			if (!Utilidades::isNumber(token) && strlen(token) > 1) {
				//Me fijo si el termino ya esta en el vector y en el documento
				//TODO BUSQUEDA BINARIA
/*				for (vector<TerminoRegister>::iterator it = terminos->begin(); it != terminos->end(); ++it) {
					if (it->getTermino().compare(token) == 0 && it->getDocumento() == nro_doc) {
						nuevo = false;
						it->addFrecuencia();
						it->addPosicion(posicion);
					}
				}
				//cout << token << endl;
//				pos = busquedaBinaria(token,terminos);
//
//				if(pos >= 0) {
//					nuevo = false;
//				}
//				if(nuevo) {
//					TerminoRegister termino;
//					termino.setDocumento(nro_doc);
//					termino.addFrecuencia();
//					termino.setTermino(token);
//					termino.addPosicion(posicion);
//					terminos->push_back(termino);
//					sort(terminos->begin(),terminos->end(),TerminoRegister::cmp);
//					(*memoriaUsada)++;
//				} else {
//					if(terminos->at(pos).getDocumento() == nro_doc){
//						terminos->at(pos).addFrecuencia();
//						terminos->at(pos).addPosicion(posicion);
//					}
//				}
				if (nuevo) {
					TerminoRegister termino;
					termino.setDocumento(nro_doc);
					termino.addFrecuencia();
					termino.setTermino(token);
					termino.addPosicion(posicion);
					terminos->push_back(termino);
					(*memoriaUsada)++;
				}*/

				TerminoRegister termino;
				termino.setDocumento(nro_doc);
				termino.setTermino(token);
				termino.setPosicion(posicion);
				terminos->push_back(termino);
				(*memoriaUsada)++;
			}
			//Tomo el siguiente token

			token = strtok(NULL, kSEPARADORES);
			if (token != NULL)
				Utilidades::string_a_minusculas(token);
			posicion++;
			nuevo = true;
		}
	}
	fin.close();
}

void imprimirArchivoParcial(vector<TerminoRegister> terminos) {
	cout << endl << "*****************Nuevo Doc Auxiliar*****************"
			<< endl;
	cout << "Termino-Documento-Frecuencia-Posiciones" << endl;
	for (vector<TerminoRegister>::iterator it = terminos.begin();
			it != terminos.end(); ++it) {

		cout << it->getTermino() << " - " << it->getDocumento() << " - "
				<< it->getFrecuencia() << " - ";
		for (vector<int>::iterator pos = it->getPosiciones()->begin();
				pos != it->getPosiciones()->end(); ++pos) {
			cout << *pos << " ";
		}
		cout << endl;
	}
}

vector<TerminoRegister> Parser::procesarTerminos(vector<TerminoRegister>* terminos) {
	vector<TerminoRegister> terminosFinal;
	vector<TerminoRegister>::const_iterator it;
	string termino = terminos->front().getTermino();
	terminosFinal.push_back(terminos->front());
	for(it = terminos->begin()+1; it != terminos->end(); ++it) {
//		cout << termino + " vs " + it->getTermino() << endl;
		//Si es un nuevo termino
		if( it->getTermino().compare(termino) != 0) {
			terminosFinal.push_back(*it);
			termino = it->getTermino();
		//Si el termino no es nuevo
		} else {

			terminosFinal.back().addFrecuencia();
			terminosFinal.back().addPosicionToList(it->getPosicion());
		}

	}
//	vector<TerminoRegister>::const_iterator itFinal;
//	for(itFinal = terminosFinal.begin(); itFinal != terminosFinal.end(); ++itFinal) {
//		vector<int>::const_iterator itPos;
//		cout << "Termino: " << itFinal->getTermino() << endl;
//		for(itPos = itFinal->getPosiciones()->begin(); itPos != itFinal->getPosiciones()->end(); ++itPos) {
//			cout << *itPos << endl;
//		}
//	}
	return terminosFinal;
}

/*
 * Recorre el directorio y procesa archivo por archivo del mismo
 */
void Parser::recorrerDirectorio(string dir, ofstream &paths, ofstream &offsets) {
	ofstream docFile;
	string filepath;
	DIR *dp;
	struct dirent *dirp;
	struct stat filestat;

	vector<TerminoRegister> terminos;

	dp = opendir(dir.c_str());

	if (dp == NULL) {
		cout << "Directorio inválido" << endl;
		return;
	}

	short nro_doc = 0;
	int memoriaUsada = 0;
	//Offset inicial: despues del header
	long offset = sizeof(short)+dir.length();



	while ((dirp = readdir(dp))) {
		filepath = dir + "/" + dirp->d_name;

		// Saltea subdirectorios
		if (stat(filepath.c_str(), &filestat)) {
			continue;
		}
		if (S_ISDIR(filestat.st_mode)) {
			continue;
		}

		//Identificacion de los documentos a indexar: guarda y numera documentos
		nro_doc++;
		guardarDocumento(dirp->d_name,nro_doc,paths,offsets,&offset);
		//TODO: comprobar si se lleno la memoria, de ser asi, bajo a disco
		if (memoriaUsada >= MAX_MEM) {
			//Ordeno la lista de terminos
			sort(terminos.begin(),terminos.end(),TerminoRegister::cmp);

			// TRAERME TERMINOS DE DISCO, JUNTARLOS CON LOS ACTUALES
			vector<TerminoRegister> terminosFinal = procesarTerminos(&terminos);

			//imprimirArchivoParcial(terminos);
			guardarEnDisco(terminosFinal);
			terminos.clear();
		}
		cout << "Procesando " + filepath << endl;
		this->processFile(filepath.c_str(), nro_doc, &terminos, &memoriaUsada);

	}
	// PARA EL ULTIMO
	//Ordeno la lista de terminos
	sort(terminos.begin(),terminos.end(),TerminoRegister::cmp);

	vector<TerminoRegister> terminosFinal = procesarTerminos(&terminos);
	//imprimirArchivoParcial(terminos);
	guardarEnDisco(terminosFinal);

	closedir(dp);
}

void Parser::guardarEnDisco(vector<TerminoRegister> terminos){

	//ACA VA EL MERGE

	std::vector<TerminoRegister>::const_iterator iterator;

	cout << "************EN MEMORIA*******************" << endl;

	Termino* termino;
	termino = new Termino(*terminos.begin());

	ofstream tIdx((repo_dir + "/terminosIdx").c_str(),ios::binary|ios::out);
	ofstream tLista((repo_dir + "/terminosLista").c_str(),ios::binary|ios::out);
	ofstream tLexico((repo_dir + "/lexico").c_str(),ios::binary|ios::out);
	ofstream tDocs(( repo_dir + "/documentos").c_str(),ios::binary|ios::out);


	for (iterator = terminos.begin(); iterator != terminos.end(); ++iterator) {

		// TERMINO NUEVO
		if(iterator->getTermino().compare(termino->palabra) != 0) {
			// Proceso termino

			IndexManager::getInstance()->indexTerm(termino,tIdx,tLista,tLexico,tDocs);

			//Aca va un delete?
			termino = new Termino(*iterator);
		}

		if(iterator->getTermino().compare("andromache") == 0) {
			cout << iterator->getDocumento() << endl;
		}
		//Con el termino, proceso el numero de documento y las posiciones
		termino->addPositionsForDoc(iterator->getDocumento(),iterator->getPosiciones());


	}

	//PROCESAR ULTIMO
	IndexManager::getInstance()->indexTerm(termino,tIdx,tLista,tLexico,tDocs);
	//VACIO EL BUFFER DE DOCUMENTOS
	ByteBuffer::getInstance()->vaciar(tDocs);
	IndexManager::getInstance()->reset();

	cout << "************GUARDADO EN DISCO*******************" << endl;

	tIdx.close();
	tLista.close();
	tLexico.close();
	tDocs.close();


}

void Parser::parsearDirectorio(string dir, string repo_dir, ofstream &paths, ofstream &offsets) {
	this->repo_dir = repo_dir;
	this->recorrerDirectorio(dir,paths,offsets);
}

void Parser::guardarDocumento(string filepath, short nro_doc, ofstream &paths, ofstream &offsets, long* offset) {
	//Formato de paths: nro (short) + #bytes(short) + path restante (variable)
	paths.write((char*)&nro_doc,sizeof(nro_doc));
	short numBytes = filepath.length();
	paths.write((char*)&numBytes,sizeof(numBytes));
	paths << filepath;

	//Formato de offsets: nroDoc (short) + offset a paths (long)
	offsets.write((char*)&nro_doc,sizeof(nro_doc));
	offsets.write((char*)offset,sizeof(*offset));
	//Se le suma al offset lo agregado en el archivo de paths
	(*offset)+=sizeof(nro_doc)+sizeof(numBytes)+filepath.length();
}


