/*
 * Coder.cpp
 *
 *  Created on: 20/05/2013
 *      Author: lara
 */

#include "Coder.h"
#include <sstream>
#include <iostream>
#include <algorithm>

Coder::Coder() {
	// TODO Auto-generated constructor stub

}

Coder::~Coder() {
	// TODO Auto-generated destructor stub
}

string Coder::encode(int x){
	return deltaCode(x);
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

string Coder::numberToBinary(int x) {
	//VERSION 1 -> malisima
//	string code;
//	int q, r;
//	if(x == 0) return "0";
//	while (x > 1) {
//		q = x / 2;
//		r = x - q * 2;
//		code = toString(r) + code;
//		x = q;
//	}
//	code = "1" + code;
//
//	return code;

	//VERSION 2 -> la buena
	if(x == 0) return "0";
	if(x == 1) return "1";
	if(x % 2 == 0) {
		return numberToBinary(x/2) + "0";
	} else {
		return numberToBinary(x/2) + "1";
	}

	//VERSION 3 -> la otra buena
//	string result = "";
//	do {
//		if( (x & 1) == 0 ) {
//			result+= "0";
//		} else {
//			result+="1";
//		}
//		x >>= 1;
//	} while(x);
//	reverse(result.begin(),result.end());
//	return result;
}

string Coder::binaryPart(int x) {
	int numToBinary = x - pow(2.0, (int) log2(x));
	int codeLength = log2(x);
	string code;
	if(codeLength > 0){
		code = numberToBinary(numToBinary);
	} else {
		code = "";
	}
	int binaryLength = code.length();
	for (int i = binaryLength; i < codeLength; i++) {
		code = "0" + code;
	}

	return code;
}

int Coder::binaryToInt(string sBinary){
	//VERSION 1
//	int pos = 0;
//	int num = 0;
//	for(int i = sBinary.length()-1; i>=0; i--){
//		if(sBinary[i]=='1'){
//			num = num + pow(2.0,pos);
//		}
//		pos++;
//	}
//	return num;

	//VERSION 2
	int result = 0;
	int pow = 1;
	for(int i = sBinary.length() - 1; i>=0; --i, pow <<= 1) {
		result+= (sBinary[i]-'0')*pow;
	}
	return result;

}

// Devuelve 0 si no existe el codigo. Guarda en tam el tamanio del codigo decodificado
int Coder::decode(string code, int* tam){
	int log = 0;
	while(code[log] == '0') {
	    log++;
	}
	string sGammaBinary;
	int i = log + 1;
	//if(code[log] != '1' || i >= code.size()) return 0;
	for(int j = 0; j<log; j++){
		sGammaBinary = sGammaBinary + code[i];
		i++;
	}
	int iGammaBinary = binaryToInt(sGammaBinary);

	int y = iGammaBinary + pow(2.0,log);

	//Leo y-1 bits mas

	string sDeltaBinary;
	for(int j = 0; j<y-1; j++){
		sDeltaBinary = sDeltaBinary + code[i];
		i++;
	}
	int iDeltaBinary = binaryToInt(sDeltaBinary);

	*tam = 2*log + y; // Longitud del codigo

	return iDeltaBinary + pow(2.0,y-1);
}
