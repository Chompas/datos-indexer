/*
 * Termino.h
 *
 *  Created on: 24/05/2013
 *      Author: lara
 */

#ifndef TERMINO_H_
#define TERMINO_H_

#include <string>
#include <vector>
#include "Coder.h"
#include "TerminoRegister.h"
using namespace std;

class Termino {
public:
	Termino(string palabra);
	Termino(TerminoRegister termino);
	virtual ~Termino();
	string palabra;
	vector<int> docs;
	vector<vector<int> > listaPosiciones;
	vector<vector<int> > listaPosicionesNormalizadas;
	string distDocs;
	vector<string> distPositions;
	void addPositionsForDoc(int doc, vector<int>* positions);
	void convertIntoDistances();
private:
	string toString(int number);


};

#endif /* TERMINO_H_ */
