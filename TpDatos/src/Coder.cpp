/*
 * Coder.cpp
 *
 *  Created on: 20/05/2013
 *      Author: lara
 */

#include "Coder.h"
#include <sstream>
#include <iostream>

Coder::Coder() {
	// TODO Auto-generated constructor stub

}

Coder::~Coder() {
	// TODO Auto-generated destructor stub
}

string toString(int number) {
	stringstream ss; //create a stringstream
	ss << number; //add number to the stream
	return ss.str(); //return a string with the contents of the stream
}

string Coder::deltaCode(int x) {

	string code;

	code = gammaCode(1 + (int) log2(x)) + binaryPart(x);

	return code;
}

string Coder::gammaCode(int x) {
	string code;

	code = unarioCode(1 + (int) log2(x)) + binaryPart(x);

	return code;
}

string Coder::unarioCode(int x) {
	string code;

	for (int i = 0; i < x - 1; i++) {
		code += "0";
	}

	return code + "1";
}

string binary(int x) {
	string code;
	int q, r;
	if(x == 0) return "0";
	while (x > 1) {
		q = x / 2;
		r = x - q * 2;
		code = toString(r) + code;
		x = q;
	}
	code = "1" + code;

	return code;
}

string Coder::binaryPart(int x) {
	int numToBinary = x - pow(2.0, (int) log2(x));
	int codeLength = log2(x);
	string code;
	if(codeLength > 0){
		code = binary(numToBinary);
	} else {
		code = "";
	}
	int binaryLength = code.length();
	for (int i = binaryLength; i < codeLength; i++) {
		code = "0" + code;
	}

	return code;
}
