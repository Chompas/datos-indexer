/*
 * Parser.h
 *
 * Clase encargada de procesar los archivos. Singleton.
 *
 *  Created on: 01/05/2013
 *      Author: lara
 */

#include <iostream>
#include <string>
#include <string.h>
#include <fstream>
#include <dirent.h>
#include <sys/stat.h>
#include <algorithm>
#include "TerminoRegister.h"
using namespace std;

#ifndef PARSER_H_
#define PARSER_H_

class Parser {
public:
	static Parser* getInstance();
	void parsearDirectorio(string dir, string repo_dir, ofstream &paths, ofstream &offsets);
private:
	static Parser* instance;
	Parser();
	void recorrerDirectorio(string dir, ofstream &paths, ofstream &offsets);
	void processFile(const char* path, short nro_doc, list<TerminoRegister>* terminos, int* memoriaUsada);
	void guardarDocumento(string filepath, short nro_doc, ofstream &paths, ofstream &offsets, long* offset);
};

#endif /* PARSER_H_ */
