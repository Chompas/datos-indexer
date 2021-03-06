/*
 * ByteBuffer.h
 *
 *  Created on: 08/06/2013
 *      Author: lara
 */

#ifndef BYTEBUFFER_H_
#define BYTEBUFFER_H_

#include <string>
using namespace std;

class ByteBuffer {
public:
	static ByteBuffer* getInstance();
	void saveBytes(string data, ofstream& tDocs);
	void vaciar(ofstream& tDocs);
private:
	ByteBuffer();
	virtual ~ByteBuffer();
	static ByteBuffer* instance;
	string bitsOcupados;
};

#endif /* BYTEBUFFER_H_ */
