/*
 * Termino.cpp
 *
 *  Created on: 24/05/2013
 *      Author: lara
 */

#include "Termino.h"
#include <iostream>
#include <sstream>

Termino::Termino(string palabra) {
	this->palabra = palabra;

}

Termino::~Termino() {
	// TODO Auto-generated destructor stub
}



//PRIVATE
string Termino::toString(int number) {
	stringstream ss; //create a stringstream
	ss << number; //add number to the stream
	return ss.str(); //return a string with the contents of the stream
}

//PUBLIC

void Termino::addPositionsForDoc(int doc, list<int>* positions) {
	this->docs.push_back(doc);
	this->listaPosiciones.push_back(*positions);
}

void Termino::convertIntoDistances() {
	std::list<int>::const_iterator iterator;
	int anterior = 0;
	this->distDocs = "";
	// Le preconcatenamos la frecuencia de documentos en Delta
	this->distDocs+=Coder::encode(docs.size());
	for (iterator = docs.begin(); iterator != docs.end(); ++iterator) {

		// DISTANCIAS EN ENTEROS
		//this->distDocs+= toString((*iterator) - anterior) + ",";

		//DISTANCIAS EN DELTA
		this->distDocs+= Coder::encode((*iterator) - anterior);
		anterior = *iterator;

	}

	std::list<list<int> >::const_iterator positionListIt;
	anterior = 0;
	for (positionListIt = listaPosiciones.begin(); positionListIt != listaPosiciones.end(); ++positionListIt){
		std::list<int>::const_iterator positionIt;
		string posDeltaAux = "";
		// Le preconcatenamos la frecuencia de cantidad de posiciones a cada lista de posiciones en DELTA
		posDeltaAux+= Coder::encode(positionListIt->size());
		for(positionIt = positionListIt->begin(); positionIt != positionListIt->end() ; ++positionIt){
			posDeltaAux+= Coder::encode((*positionIt) - anterior);
			anterior = *positionIt;
		}
		this->distPositions.push_back(posDeltaAux);
		cout << posDeltaAux << endl;
		posDeltaAux = "";
		anterior = 0;

	}

	cout << endl;

}
