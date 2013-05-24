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
using namespace std;

class Termino {
public:
	Termino(string palabra);
	virtual ~Termino();
	string palabra;
	list<int> docs;
	list<list<int> > posiciones;
	void addPositionsForDoc(int doc, list<int>* positions);


};

#endif /* TERMINO_H_ */
