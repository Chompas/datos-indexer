/*
 * TerminoRegister.h
 *
 * Clase que contiene los datos necesarios de cada termino
 *
 *  Created on: 01/05/2013
 *      Author: lara
 */

#include <string>
#include <list>
#include <iostream>
using namespace std;

#ifndef TERMINOREGISTER_H_
#define TERMINOREGISTER_H_

class TerminoRegister {
public:
	TerminoRegister();
	virtual ~TerminoRegister();

	static bool cmp(TerminoRegister i, TerminoRegister j);

	int getDocumento() const {
		return documento;
	}

	void setDocumento(int documento) {
		this->documento = documento;
	}

	int getFrecuencia() const {
		return frecuencia;
	}

	void addFrecuencia() {
		this->frecuencia++;
	}

	list<int>* getPosiciones() const {
		return posiciones;
	}

	void addPosicion(int posicion) {
		posiciones->push_back(posicion);
	}

	string getTermino() const {
		return termino;
	}

	void setTermino(string termino) {
		this->termino = termino;
	}

private:
	string termino;
	int documento;
	int frecuencia;
	list<int>* posiciones;
};

#endif /* TERMINOREGISTER_H_ */
