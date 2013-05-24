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
private:
	IndexManager();
	static IndexManager* instance;
	int docOffset;
	int termOffset;
	int lexicOffset;
	string toString(int number);

};

#endif /* INDEXMANAGER_H_ */
