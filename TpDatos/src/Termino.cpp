/*
 * Termino.cpp
 *
 *  Created on: 24/05/2013
 *      Author: lara
 */

#include "Termino.h"

Termino::Termino(string palabra) {
	this->palabra = palabra;
}

Termino::~Termino() {
	// TODO Auto-generated destructor stub
}

void Termino::addPositionsForDoc(int doc, list<int>* positions) {
	this->docs.push_back(doc);
	this->posiciones.push_back(*positions);
}
