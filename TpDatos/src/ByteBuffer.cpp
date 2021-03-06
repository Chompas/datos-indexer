/*
 * ByteBuffer.cpp
 *
 *  Created on: 08/06/2013
 *      Author: lara
 */

#include "ByteBuffer.h"
#include "FileManager.h"
#include "Coder.h"

ByteBuffer::ByteBuffer() {
	string bitsOcupados = "";

}

ByteBuffer::~ByteBuffer() {
	// TODO Auto-generated destructor stub
}

ByteBuffer* ByteBuffer::instance = 0;

ByteBuffer* ByteBuffer::getInstance() {
	if (instance == 0) {
		instance = new ByteBuffer();
	}
	return instance;
}

void ByteBuffer::saveBytes(string data, ofstream& tDocs) {
	string byteAGuardarAux;
	unsigned char byte;
	int size,pos = 0;

	while(bitsOcupados.length() + data.length() - pos  >= 8) {
		//Agarro los bits de data para completar el byte
		size = 8 - bitsOcupados.length();
		byteAGuardarAux = bitsOcupados + data.substr(pos, size);
		//Los convierto a un numero y los guardo en el archivo
		byte = (unsigned char)Coder::binaryToInt(byteAGuardarAux);
		FileManager::getInstance()->saveToFile(&byte,sizeof(byte),tDocs);
		byteAGuardarAux.clear();
		bitsOcupados.clear();
		pos+= size;
	}
	bitsOcupados+= data.substr(pos,data.length()-pos);
}

void ByteBuffer::vaciar(ofstream& tDocs) {
	//Completo el byte con 0s
	int len;
	if(bitsOcupados.length()>0) {
		unsigned char byte;
		len = bitsOcupados.length();
		for(int i = 0; i < 8 - len; i++){
			bitsOcupados+="0";
		}
		byte = (unsigned char)Coder::binaryToInt(bitsOcupados);
		FileManager::getInstance()->saveToFile(&byte,sizeof(byte),tDocs);
		bitsOcupados.clear();
	}
}
