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

Termino::Termino(TerminoRegister termino) {
	this->palabra = termino.getTermino();
//	this->docs
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

void Termino::addPositionsForDoc(int doc, long position) {
	if(this->docs.size() == 0){
		this->docs.push_back(doc);
		vector<long> positions;
		positions.push_back(position);
		this->listaPosiciones.push_back(positions);
	}else if(this->docs.back() < doc) {

		this->docs.push_back(doc);
		vector<long> positions;
		positions.push_back(position);
		this->listaPosiciones.push_back(positions);

	} else {
		this->listaPosiciones.back().push_back(position);
	}
}

void Termino::addPositionsForDoc(int doc, vector<long> positions) {
	this->docs.push_back(doc);
	this->listaPosiciones.push_back(positions);
}

void Termino::convertIntoDistances() {
	std::vector<int>::const_iterator iterator;
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

	std::vector<vector<long> >::const_iterator positionListIt;
	anterior = 0;
	for (positionListIt = listaPosiciones.begin(); positionListIt != listaPosiciones.end(); ++positionListIt){
		std::vector<long>::const_iterator positionIt;
		string posDeltaAux = "";
		// Le preconcatenamos la frecuencia de cantidad de posiciones a cada lista de posiciones en DELTA
		posDeltaAux+= Coder::encode(positionListIt->size());
		for(positionIt = positionListIt->begin(); positionIt != positionListIt->end() ; ++positionIt){
			posDeltaAux+= Coder::encode((*positionIt) - anterior);
			anterior = *positionIt;
		}
		this->distPositions.push_back(posDeltaAux);
		posDeltaAux = "";
		anterior = 0;

	}


}
