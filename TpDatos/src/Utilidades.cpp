/*
 * Utilidades.cpp
 *
 *  Created on: 12/06/2013
 *      Author: lara
 */

#include "Utilidades.h"

Utilidades::Utilidades() {
	// TODO Auto-generated constructor stub

}

Utilidades::~Utilidades() {
	// TODO Auto-generated destructor stub
}

bool Utilidades::isNumber(string s) {
	std::string::const_iterator it = s.begin();
	while (it != s.end() && std::isdigit(*it))
		++it;
	return !s.empty() && it == s.end();
}
void Utilidades::string_a_minusculas(char* string) {
	for (int i = 0; string[i] != '\0'; i++) {
		string[i] = (char) tolower(string[i]);
	}
}
