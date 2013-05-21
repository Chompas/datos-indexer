/*
 * Coder.h
 *
 *  Created on: 20/05/2013
 *      Author: lara
 */

#ifndef CODER_H_
#define CODER_H_

#include <string>
#include <cmath>
using namespace std;


class Coder {
public:
	Coder();
	virtual ~Coder();
	string encode(int x);
private:
	string deltaCode(int x);
	string gammaCode(int x);
	string unarioCode(int x);
	string binaryPart(int x);
};

#endif /* CODER_H_ */
