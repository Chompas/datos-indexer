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
	static string encode(int x);
private:
	static string deltaCode(int x);
	static string gammaCode(int x);
	static string unarioCode(int x);
	static string binaryPart(int x);
};

#endif /* CODER_H_ */
