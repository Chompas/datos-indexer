/*
 * Parser.cpp
 *
 *  Created on: 01/05/2013
 *      Author: lara
 */

#include "Parser.h"
#include <sstream>
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

/*
 * Recorre el archivo y va separando en tokens cada linea. Asigna los terminos al vector de terminos
 */
void Parser::processFile(const char* path, short nro_doc,
		list<TerminoRegister>* terminos, int* memoriaUsada) {
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
		while (token != NULL) {
			if (!Utilidades::isNumber(token) && strlen(token) > 1) {
				//Me fijo si el termino ya esta en el vector y en el documento
				//TODO BUSQUEDA BINARIA
				for (list<TerminoRegister>::iterator it = terminos->begin(); it != terminos->end(); ++it) {
					if (it->getTermino().compare(token) == 0 && it->getDocumento() == nro_doc) {
						nuevo = false;
						it->addFrecuencia();
						it->addPosicion(posicion);
					}
				}
				if (nuevo) {
					TerminoRegister termino;
					termino.setDocumento(nro_doc);
					termino.addFrecuencia();
					termino.setTermino(token);
					termino.addPosicion(posicion);
					terminos->push_back(termino);
					(*memoriaUsada)++;
				}
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

void imprimirArchivoParcial(list<TerminoRegister> terminos) {
	cout << endl << "*****************Nuevo Doc Auxiliar*****************"
			<< endl;
	cout << "Termino-Documento-Frecuencia-Posiciones" << endl;
	for (list<TerminoRegister>::iterator it = terminos.begin();
			it != terminos.end(); ++it) {

		cout << it->getTermino() << " - " << it->getDocumento() << " - "
				<< it->getFrecuencia() << " - ";
		for (list<int>::iterator pos = it->getPosiciones()->begin();
				pos != it->getPosiciones()->end(); ++pos) {
			cout << *pos << " ";
		}
		cout << endl;
	}
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

	list<TerminoRegister> terminos;

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
			terminos.sort(TerminoRegister::cmp);

			imprimirArchivoParcial(terminos);
			guardarEnDisco(terminos);
			terminos.clear();
		}
		cout << "Procesando " + filepath << endl;
		this->processFile(filepath.c_str(), nro_doc, &terminos, &memoriaUsada);

	}
	// PARA EL ULTIMO
	//Ordeno la lista de terminos
	terminos.sort(TerminoRegister::cmp);

	imprimirArchivoParcial(terminos);
	guardarEnDisco(terminos);

	closedir(dp);
}

void Parser::guardarEnDisco(list<TerminoRegister> terminos){

	std::list<TerminoRegister>::const_iterator iterator;

	cout << "************GUARDADO EN DISCO*******************" << endl;

	Termino* termino;
	termino = new Termino(terminos.begin()->getTermino());
	for (iterator = terminos.begin(); iterator != terminos.end(); ++iterator) {

		if(iterator->getTermino().compare(termino->palabra) != 0) {
			// Proceso termino

			IndexManager::getInstance()->indexTerm(termino,repo_dir);

			//Aca va un delete?
			termino = new Termino(iterator->getTermino());
		}

		//Con el termino, proceso el numero de documento y las posiciones
		termino->addPositionsForDoc(iterator->getDocumento(),iterator->getPosiciones());


	}

	//PROCESAR ULTIMO
	IndexManager::getInstance()->indexTerm(termino,repo_dir);
	//VACIO EL BUFFER DE DOCUMENTOS
	ByteBuffer::getInstance()->vaciar(repo_dir+"/"+kDOCUMENTOS);
	IndexManager::getInstance()->reset();


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


