/*
 * TerminoRegister.cpp
 *
 *  Created on: 01/05/2013
 *      Author: lara
 */

#include "TerminoRegister.h"

TerminoRegister::TerminoRegister() {
	this->frecuencia = 1;
	this->documento = 0;
	this->posicion = 0;

}

TerminoRegister::TerminoRegister(string termino,int doc, int pos) {
	this->termino = termino;
	this->documento = doc;
	this->posicion = pos;
}

TerminoRegister::~TerminoRegister() {
	// TODO Auto-generated destructor stub
}

/*
 * Funcion de comparacion para ordenar primero por nombre, desupues por numero de documento, y despues por posicion
 */
bool TerminoRegister::cmp(TerminoRegister i, TerminoRegister j) {

	string iTermino = i.getTermino(), jTermino = j.getTermino();
	int iDoc = i.getDocumento(), jDoc = j.getDocumento();
	long iPos = i.getPosicion(), jPos = j.getPosicion();
	if(	iTermino < jTermino){
		return true;
	} else if(iTermino == jTermino && iDoc < jDoc) {
		return true;
	} else if(iTermino == jTermino && iDoc == jDoc && iPos < jPos) {
		return true;
	}
	return false;
}
