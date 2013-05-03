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

void string_a_minusculas(char* string) {
	for (int i = 0; string[i] != '\0'; i++) {
		string[i] = (char) tolower(string[i]);
	}
}

/*
 * Recorre el archivo y va separando en tokens cada linea. Asigna los terminos al vector de terminos
 */
void Parser::processFile(const char* path, int nro_doc, list<TerminoRegister>* terminos, int* memoriaUsada) {
	string line;
	ifstream fin;
	fin.open(path);
	int posicion = 1;
	while (!fin.eof()) {

		getline(fin, line);
		char* token = strtok((char*) line.c_str(), " ,.;~{}[]+-_=?¿!¡/@#$%&¬()<>€çÇ");
		if (token != NULL)
			string_a_minusculas(token);
		bool nuevo = true;
		while (token != NULL) {

			//Me fijo si el termino ya esta en el vector y en el documento
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

			//Tomo el siguiente token
			token = strtok(NULL, " ,.;~");
			if (token != NULL)
				string_a_minusculas(token);
			posicion++;
			nuevo = true;
		}
	}
	fin.close();
}

void imprimirArchivoParcial(list<TerminoRegister> terminos) {
	cout << endl << "*****************Nuevo Doc Auxiliar*****************" << endl;
	cout << "Termino\tDocumento\tFrecuencia\tPosiciones" << endl;
	for (list<TerminoRegister>::iterator it = terminos.begin();
			it != terminos.end(); ++it) {

		cout << it->getTermino() << "\t" << it->getDocumento() << "\t"
				<< it->getFrecuencia() << "\t";
		for (list<int>::iterator pos = it->getPosiciones()->begin();
				pos != it->getPosiciones()->end(); ++pos) {
			cout << *pos << ",";
		}
		cout << endl;
	}
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

	list<TerminoRegister> terminos;

	dp = opendir(dir.c_str());

	if (dp == NULL) {
		cout << "Directorio inválido" << endl;
		return;
	}

	int nro_doc = 0;
	int memoriaMaxima = 32;
	int memoriaUsada = 0;

	while ((dirp = readdir(dp))) {
		filepath = dir + "/" + dirp->d_name;

		// Saltea subdirectorios
		if (stat(filepath.c_str(), &filestat)) {
			continue;
		}
		if (S_ISDIR(filestat.st_mode)) {
			continue;
		}

		//TODO: Identificacion de los documentos a indexar: guardar y numerar documentos
		nro_doc++;
		//TODO: comprobar si se lleno la memoria, de ser asi, bajo a disco
		if(memoriaUsada >= memoriaMaxima) {
			//Ordeno la lista de terminos
			terminos.sort(TerminoRegister::cmp);
			//guardarEnDisco(terminos);
			imprimirArchivoParcial(terminos);
			terminos.clear();
		}
		cout << "Procesando " + filepath << endl;
		this->processFile(filepath.c_str(), nro_doc, &terminos, &memoriaUsada);

	}
	// PARA EL ULTIMO
	//Ordeno la lista de terminos
	terminos.sort(TerminoRegister::cmp);
	//guardarEnDisco(terminos);
	imprimirArchivoParcial(terminos);

	closedir(dp);
}

void Parser::parsearDirectorio(string dir) {
	this->recorrerDirectorio(dir);
}
