/*
 * Parser.cpp
 *
 *  Created on: 01/05/2013
 *      Author: lara
 */

#include "Parser.h"

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
void Parser::processFile(const char* path, int nro_doc,
		vector<TerminoRegister>* terminos) {
	string line;
	ifstream fin;
	fin.open(path);
	int posicion = 1;
	while (!fin.eof()) {

		getline(fin, line);
		char* token = strtok((char*) line.c_str(), " ,.;~");
		bool nuevo = true;
		while (token != NULL) {
			//Me fijo si el termino ya esta en el vector
			for (vector<TerminoRegister>::iterator it = terminos->begin();
					it != terminos->end(); ++it) {
				if (it->getTermino().compare(token) == 0
						&& it->getDocumento() == nro_doc) {
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
			}
			token = strtok(NULL, " ,.;~");
			posicion++;
			nuevo = true;
		}
	}
	fin.close();
}

/*
 * Recorre el directorio y procesa archivo por archivo del mismo
 */
void Parser::recorrerDirectorio(string dir) {
	ifstream fin;
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

	int nro_doc = 1;

	while ((dirp = readdir(dp))) {
		filepath = dir + "/" + dirp->d_name;

		// Saltea subdirectorios
		if (stat(filepath.c_str(), &filestat)) {
			continue;
		}
		if (S_ISDIR(filestat.st_mode)) {
			continue;
		}

		cout << "Procesando " + filepath << endl;
		this->processFile(filepath.c_str(), nro_doc, &terminos);
		nro_doc++;
	}

	//Ordeno el vector de terminos
	sort(terminos.begin(),terminos.end(),TerminoRegister::cmp);

	//Bajo a disco los terminos (por ahora los imprimo en pantalla)
	cout << "Termino\tDocumento\tFrecuencia\tPosiciones" << endl;
	for (vector<TerminoRegister>::iterator it = terminos.begin();
			it != terminos.end(); ++it) {

		cout << it->getTermino() << "\t" << it->getDocumento() << "\t"
				<< it->getFrecuencia() << "\t";
		for (vector<int>::iterator pos = it->getPosiciones()->begin();
				pos != it->getPosiciones()->end(); ++pos) {
			cout << *pos << ",";
		}
		cout << endl;
	}

	closedir(dp);
}

void Parser::parsearDirectorio(string dir) {
	this->recorrerDirectorio(dir);
}
