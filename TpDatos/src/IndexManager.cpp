/*
 * IndexManager.cpp
 *
 *  Created on: 24/05/2013
 *      Author: lara
 */

#include "IndexManager.h"
#include "FileManager.h"
#include <sstream>

#define kDOCUMENTOS 	"documentos"
#define kTERMINOS		"terminosIdx"
#define kLEXICO			"lexico"
#define kLISTATERMINOS	"terminosLista"


IndexManager* IndexManager::instance = 0;


IndexManager::IndexManager() {
	this->docOffset = 0;
	this->termOffset = 0;
	this->lexicOffset = 0;

}

IndexManager::~IndexManager() {
	// TODO Auto-generated destructor stub
}

IndexManager* IndexManager::getInstance() {
	if (instance == 0) {
		instance = new IndexManager();
	}
	return instance;
}

string IndexManager::toString(int number) {
	stringstream ss; //create a stringstream
	ss << number; //add number to the stream
	return ss.str(); //return a string with the contents of the stream
}

void IndexManager::indexTerm(Termino* termino, string repo_dir) {
	//Pasar docs y posiciones a distancias

	termino->convertIntoDistances();

	//Guardar el termino en disco

	string docRegister = "";

	docRegister+=termino->distDocs;

	std::list<string>::const_iterator positionIt;
	for(positionIt = termino->distPositions.begin();positionIt != termino->distPositions.end(); ++positionIt ) {
		docRegister+=*positionIt;
	}

	// Guardo registro en el archivo de documentos
	FileManager::getInstance()->saveToFile(docRegister,repo_dir+"/"+kDOCUMENTOS);

	//Guardo termino en el archivo de terminos completos o en el de lexico
	string terminoCompleto = 	toString(termino->palabra.size()) +
								toString(termOffset) +
								toString(lexicOffset) +
								toString(docOffset);
	FileManager::getInstance()->saveToFile(terminoCompleto,repo_dir+"/"+kTERMINOS);

	FileManager::getInstance()->saveToFile(termino->palabra,repo_dir+"/"+kLISTATERMINOS);


	this->docOffset+=docRegister.size();
	this->termOffset+=termino->palabra.size();
	//this->lexicOffset = lexicRegister.size();

}
