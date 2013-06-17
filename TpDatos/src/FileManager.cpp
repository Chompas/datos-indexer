/*
 * FileManager.cpp
 *
 *  Created on: 24/05/2013
 *      Author: lara
 */

#include "FileManager.h"
#include "Utilidades.h"
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

void FileManager::guardarArchivoTemporal(vector<TerminoRegister> terminos,string repo_dir, int i) {
	vector<TerminoRegister>::const_iterator it;
	ofstream out((repo_dir+"/"+Utilidades::toString(i)).c_str(),ios::out|ios::binary);
	for(it = terminos.begin(); it != terminos.end(); ++it) {
		int size = it->getTermino().size();
		saveToFile(&size,sizeof(size),out);
		saveToFile(it->getTermino(),out);
		int doc = it->getDocumento();
		int pos = it->getPosicion();
		saveToFile(&doc,sizeof(doc),out);
		saveToFile(&pos,sizeof(pos),out);
	}
	out.close();
}

void FileManager::guardarStreamTemporal(TerminoRegister termino,ofstream& out) {
	int size = termino.getTermino().size();
	saveToFile(&size,sizeof(size),out);
	saveToFile(termino.getTermino(),out);
	int doc = termino.getDocumento();
	int pos = termino.getPosicion();
	saveToFile(&doc,sizeof(doc),out);
	saveToFile(&pos,sizeof(pos),out);
}

TerminoRegister FileManager::leerTermino(ifstream& in){
	TerminoRegister termino;
	int size1;
	in.read((char*)&size1,sizeof(size1));
	if(in.fail()) {
		termino = TerminoRegister("",0,0);
	} else {
		char palabra1[size1];
		in.read(palabra1,size1);
		string palabra = palabra1;
		palabra = palabra.substr(0,size1);
		int doc1;
		in.read((char*)&doc1,sizeof(doc1));
		int pos1;
		in.read((char*)&pos1,sizeof(pos1));
		termino = TerminoRegister(palabra,doc1,pos1);
	}
	return termino;
}
