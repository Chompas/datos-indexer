/*
 * Utilidades.h
 *
 *  Created on: 12/06/2013
 *      Author: lara
 */

#ifndef UTILIDADES_H_
#define UTILIDADES_H_

#include <string>
using namespace std;

class Utilidades {
public:
	Utilidades();
	virtual ~Utilidades();
	static bool isNumber(string s);
	static void string_a_minusculas(char* string);
	static string toString(int number);
};

#endif /* UTILIDADES_H_ */
