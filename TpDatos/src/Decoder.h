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
	int decode(string code);
	int binaryToInt(string sBinary);
};

#endif /* DECODER_H_ */
