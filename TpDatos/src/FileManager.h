/*
 * FileManager.h
 *
 *  Created on: 24/05/2013
 *      Author: lara
 */

#ifndef FILEMANAGER_H_
#define FILEMANAGER_H_

#include "TerminoRegister.h"
#include <string>
using namespace std;

class FileManager {
public:
	static FileManager* getInstance();
	virtual ~FileManager();
	static int saveToFile(string buffer,ofstream& filestream);
	static int saveToFile(void* buffer, int size, ofstream& filestream);
	static void guardarArchivoTemporal(vector<TerminoRegister> terminos,string repo_dir, int i);
	static TerminoRegister leerTermino(ifstream& in);
	static void guardarStreamTemporal(TerminoRegister termino,ofstream& out);
private:
	static FileManager* instance;
	FileManager();
};

#endif /* FILEMANAGER_H_ */
