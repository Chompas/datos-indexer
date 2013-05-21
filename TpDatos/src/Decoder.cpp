/*
 * Decoder.cpp
 *
 *  Created on: 20/05/2013
 *      Author: lara
 */

#include "Decoder.h"
#include <iostream>

Decoder::Decoder() {
	// TODO Auto-generated constructor stub

}

Decoder::~Decoder() {
	// TODO Auto-generated destructor stub
}

int Decoder::binaryToInt(string sBinary){
	int pos = 0;
	int num = 0;
	for(int i = sBinary.length()-1; i>=0; i--){
		if(sBinary[i]=='1'){
			num = num + pow(2.0,pos);
		}
		pos++;
	}
	return num;

}

int Decoder::decode(string code){
	int log = 0;
	while(code[log] == '0') {
	    log++;
	}
	string sGammaBinary;
	int i = log + 1;
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

	return iDeltaBinary + pow(2.0,y-1);
}
