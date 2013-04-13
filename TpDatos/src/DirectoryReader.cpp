/*
 * DirectoryReader.cpp
 *
 *  Created on: 12/04/2013
 *      Author: lara
 */

#include "DirectoryReader.h"

DirectoryReader* DirectoryReader::instance = 0;

DirectoryReader::DirectoryReader() {

}

DirectoryReader* DirectoryReader::getInstance() {
	if (instance == 0) {
		instance = new DirectoryReader();
	}
	return instance;
}

void DirectoryReader::recorrerDirectorio(string dir) {
	ifstream fin;
	string filepath;
	DIR *dp;
	struct dirent *dirp;
	struct stat filestat;

	dp = opendir(dir.c_str());

	if (dp == NULL) {
		cout << "Directorio inválido" << endl;
		return;
	}

	while ((dirp = readdir(dp))) {
		filepath = dir + "/" + dirp->d_name;

		// Saltea subdirectorios
		if (stat(filepath.c_str(), &filestat)) {
			continue;
		}
		if (S_ISDIR(filestat.st_mode)) {
			continue;
		}

		fin.open(filepath.c_str());
		cout << "Procesando " + filepath << endl;
		//		readFile(filepath);
		fin.close();
	}

	closedir(dp);
}

