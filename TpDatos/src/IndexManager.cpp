/*
 * IndexManager.cpp
 *
 *  Created on: 24/05/2013
 *      Author: lara
 */

#include "IndexManager.h"
#include "FileManager.h"
#include <sstream>
#include <iostream>

#define kDOCUMENTOS 	"documentos"
#define kTERMINOS		"terminosIdx"
#define kLEXICO			"lexico"
#define kLISTATERMINOS	"terminosLista"

#define kNFRONTCODING	5


IndexManager* IndexManager::instance = 0;


IndexManager::IndexManager() {
	this->docOffset = 0;
	this->termOffset = 0;
	this->lexicOffset = 0;
	this->cantPalabras = 0;
	this->lastTerminoCompleto = "";

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

void IndexManager::saveTerminoCompleto(Termino* termino,string repo_dir) {
	string terminoCompleto = 	toString(termino->palabra.size()) +
								toString(termOffset) +
								toString(lexicOffset) +
								toString(docOffset);
	FileManager::getInstance()->saveToFile(terminoCompleto,repo_dir+"/"+kTERMINOS);

	FileManager::getInstance()->saveToFile(termino->palabra,repo_dir+"/"+kLISTATERMINOS);

	this->lastTerminoCompleto = termino->palabra;

}

void IndexManager::saveLexico(Termino* termino, string repo_dir) {
	bool distintos = false;
	unsigned int iguales = 0;
	cout << termino->palabra << " - ";
	cout << lastTerminoCompleto << endl;
	while(!distintos && (iguales <= termino->palabra.size() || iguales<=lastTerminoCompleto.size())) {
		cout << termino->palabra[iguales] << " - " << lastTerminoCompleto[iguales] << endl;
		if(termino->palabra[iguales] != lastTerminoCompleto[iguales]) {
			distintos = true;
		} else {
			iguales++;
		}
	}

	int cantCaractDistintos = termino->palabra.size() - iguales;
	string lexicoRegister = toString(iguales) + toString(cantCaractDistintos)+ &(termino->palabra[iguales])+toString(docOffset);

	FileManager::getInstance()->saveToFile(lexicoRegister,repo_dir+"/"+kLEXICO);
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

	if(cantPalabras % kNFRONTCODING == 0) {
		saveTerminoCompleto(termino,repo_dir);
	} else {
		saveLexico(termino,repo_dir);
	}


	this->cantPalabras++;
	this->docOffset+=docRegister.size();
	this->termOffset+=termino->palabra.size();
	//this->lexicOffset = lexicRegister.size();

}
