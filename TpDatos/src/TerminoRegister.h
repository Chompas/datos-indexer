/*
 * TerminoRegister.h
 *
 * Clase que contiene los datos necesarios de cada termino
 *
 *  Created on: 01/05/2013
 *      Author: lara
 */

#include <string>
#include <iostream>
#include <vector>
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

	vector<long> getPosiciones() const {
		return listaPosiciones;
	}

	void addPosicionToList(long posicion) {
		listaPosiciones.push_back(posicion);
	}

	string getTermino() const {
		return termino;
	}

	void setTermino(string termino) {
		this->termino = termino;
	}
	long getPosicion() const {
		return posicion;
	}

	void setPosicion(long posicion) {
		listaPosiciones.push_back(posicion);
		this->posicion = posicion;
	}

	void inicializarPosiciones() {
		listaPosiciones.push_back(this->posicion);
	}

private:
	string termino;
	int documento;
	int frecuencia;
	long posicion;
	vector<long> listaPosiciones;
};

#endif /* TERMINOREGISTER_H_ */
