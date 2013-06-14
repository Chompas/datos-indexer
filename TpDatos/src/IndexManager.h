/*
 * IndexManager.h
 *
 *  Created on: 24/05/2013
 *      Author: lara
 */

#ifndef INDEXMANAGER_H_
#define INDEXMANAGER_H_

#include "Termino.h"

class IndexManager {
public:
	static IndexManager* getInstance();
	virtual ~IndexManager();
	void indexTerm(Termino* termino, ofstream& tIdxIn,ofstream& tListaIn,ofstream& tLexico,ofstream& tDocs);
	void reset();
private:
	IndexManager();
	static IndexManager* instance;
	int docOffset;
	int termOffset;
	int lexicOffset;
	string lastTerminoCompleto;
	string toString(int number);
	int cantPalabras;
	void saveTerminoCompleto(Termino* termino,ofstream& tIdx, ofstream& tLista);
	void saveLexico(Termino* termino, ofstream& tLexico);

};

#endif /* INDEXMANAGER_H_ */
