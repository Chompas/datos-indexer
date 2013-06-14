/*
 * FileManager.cpp
 *
 *  Created on: 24/05/2013
 *      Author: lara
 */

#include "FileManager.h"
#include <fstream>

FileManager* FileManager::instance = 0;

FileManager::FileManager() {
	// TODO Auto-generated constructor stub

}

FileManager::~FileManager() {
	// TODO Auto-generated destructor stub
}


FileManager* FileManager::getInstance(){
	if (instance == 0) {
		instance = new FileManager();
	}
	return instance;}

int FileManager::saveToFile(string buffer,ofstream& filestream ){
	filestream << buffer;
	//Habria que devolver si se pudo instertar
	return 0;
}

int FileManager::saveToFile(void* buffer, int size, ofstream& filestream) {
	filestream.write((char*)buffer,size);
	return 0;
}
