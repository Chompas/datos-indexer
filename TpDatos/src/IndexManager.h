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
	void indexTerm(Termino* termino, string repo_dir);
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
	void saveTerminoCompleto(Termino* termino,string repo_dir);
	void saveLexico(Termino* termino, string repo_dir);

};

#endif /* INDEXMANAGER_H_ */
