/*
 * Parser.cpp
 *
 *  Created on: 01/05/2013
 *      Author: lara
 */

#include "Parser.h"
#include <sstream>

int MAX_MEM = 64;

string separadores = " ,.;:~{}[]+-_=?¿!¡/@#$%&¬()<>€çÇ\t\n\"'±¹²³ºª·";

Parser* Parser::instance = 0;

Parser::Parser() {

}

Parser* Parser::getInstance() {
	if (instance == 0) {
		instance = new Parser();
	}
	return instance;
}

bool isNumber(string s) {
	std::string::const_iterator it = s.begin();
	while (it != s.end() && std::isdigit(*it))
		++it;
	return !s.empty() && it == s.end();
}
void string_a_minusculas(char* string) {
	for (int i = 0; string[i] != '\0'; i++) {
		string[i] = (char) tolower(string[i]);
	}
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
		char* token = strtok((char*) line.c_str(), separadores.c_str());
		if (token != NULL)
			string_a_minusculas(token);
		bool nuevo = true;
		while (token != NULL) {
			if (!isNumber(token) && strlen(token) > 1) {
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

			token = strtok(NULL, separadores.c_str());
			if (token != NULL)
				string_a_minusculas(token);
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

void Parser::parsearDirectorio(string dir, string repo_dir, ofstream &paths, ofstream &offsets) {
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


