/*
 * Termino.h
 *
 *  Created on: 24/05/2013
 *      Author: lara
 */

#ifndef TERMINO_H_
#define TERMINO_H_

#include <string>
#include <list>
#include "Coder.h"
using namespace std;

class Termino {
public:
	Termino(string palabra);
	virtual ~Termino();
	string palabra;
	list<int> docs;
	list<list<int> > listaPosiciones;
	string distDocs;
	list<string> distPositions;
	void addPositionsForDoc(int doc, list<int>* positions);
	void convertIntoDistances();
private:
	string toString(int number);


};

#endif /* TERMINO_H_ */
