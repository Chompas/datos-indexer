/*
 * IndexManager.cpp
 *
 *  Created on: 24/05/2013
 *      Author: lara
 */

#include "IndexManager.h"
#include "FileManager.h"
#include "Defines.h"
#include <sstream>
#include <fstream>
#include <iostream>




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
	//FileManager::getInstance()->saveToFile(terminoCompleto,repo_dir+"/"+kTERMINOS);

	// *******************ESTO LO TENDRIA QUE HACER EL FILE MANAGER*****************************
	short size;
	size = termino->palabra.size(); // Transformo el size a short para que ocupe 2 bytes
	ofstream out((repo_dir+"/"+kTERMINOS).c_str(),ios::binary | ios::app);
	out.write((char*)&size,sizeof(size));
	out.write((char*)&termOffset,sizeof(termOffset));
	out.write((char*)&lexicOffset,sizeof(lexicOffset));
	out.write((char*)&docOffset,sizeof(docOffset));
	out.close();

//	FileManager::getInstance()->saveToFile(size,repo_dir+"/"+kTERMINOS);
//	FileManager::getInstance()->saveToFile(termOffset,repo_dir+"/"+kTERMINOS);
//	FileManager::getInstance()->saveToFile(lexicOffset,repo_dir+"/"+kTERMINOS);
//	FileManager::getInstance()->saveToFile(docOffset,repo_dir+"/"+kTERMINOS);

	FileManager::getInstance()->saveToFile(termino->palabra,repo_dir+"/"+kLISTATERMINOS);

	this->lastTerminoCompleto = termino->palabra;

}

void IndexManager::saveLexico(Termino* termino, string repo_dir) {
	bool distintos = false;
	unsigned short iguales = 0;
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

	unsigned short cantCaractDistintos = termino->palabra.size() - iguales;
//	string lexicoRegister = toString(iguales) + toString(cantCaractDistintos)+ &(termino->palabra[iguales])+toString(docOffset);


	//FileManager::getInstance()->saveToFile(lexicoRegister,repo_dir+"/"+kLEXICO);


	/*****************************ESTO LO TENDRIA QUE HACER EL FILE MANAGER**************************/
	ofstream out((repo_dir+"/"+kLEXICO).c_str(),ios::binary | ios::app);
	out.write((char*)&iguales,sizeof(iguales));
	out.write((char*)&cantCaractDistintos,sizeof(cantCaractDistintos));
	out.write(&(termino->palabra[iguales]),cantCaractDistintos);
	out.write((char*)&docOffset,sizeof(docOffset));
	out.close();


	/* Actualizacion del offset de lexico
	 * CADA REGISTRO DE LEXICO TIENE:
	 * 		- 2 BYTES PARA REPETIDOS
	 * 		- 2 BYTES PARA DISTINTOS
	 * 		- CARACTERES DISTINTOS (VARIABLE)
	 * 		- 4 BYTES PARA OFFSET A ENTEROS
	 */
	this->lexicOffset+=2+2+cantCaractDistintos+4;
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
		//El offset de termino se actualiza cada vez que cambio el termino completo
		this->termOffset+=termino->palabra.size();
	} else {
		//El offset de lexico se actualiza dentro de saveLexico ya que necesito la cantidad de caracteres distintos
		saveLexico(termino,repo_dir);
	}


	this->cantPalabras++;
	// El offset de documentos se actualiza siempre
	this->docOffset+=docRegister.size();

}

void IndexManager::reset() {
	this->docOffset = 0;
	this->termOffset = 0;
	this->lexicOffset = 0;
	this->cantPalabras = 0;
	this->lastTerminoCompleto = "";
}
