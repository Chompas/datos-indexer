/*
 * Decoder.h
 *
 *  Created on: 20/05/2013
 *      Author: lara
 */

#ifndef DECODER_H_
#define DECODER_H_

#include <string>
#include <cmath>
#include <cstring>
using namespace std;

class Decoder {
public:
	Decoder();
	virtual ~Decoder();
	static int decode(string code);
private:
	static int binaryToInt(string sBinary);
};

#endif /* DECODER_H_ */
