/*
 * IndexManager.h
 *
 *  Created on: 24/05/2013
 *      Author: lara
 */

#ifndef INDEXMANAGER_H_
#define INDEXMANAGER_H_

#include "Termino.h"

class IndexManager {
public:
	static IndexManager* getInstance();
	virtual ~IndexManager();
	void indexTerm(Termino* termino, ofstream& tIdxIn,ofstream& tListaIn,ofstream& tLexico,ofstream& tDocs);
	void reset();
	string obtenerTermino(ifstream& tIdxIn, string listaTerminosCompletos, int registro);
	void intersecarPosiciones(vector<long>* interseccion, vector<long> posicionesAIntersecar);
	void intersecar(vector<int>* interseccion, vector<int> docsAIntersecar);
	Termino* decodeDocRegister(string palabra, string docs);
	string obtenerDocs(ifstream& tDocs, int docOffset, int docOffsetSiguiente);
	int busquedaEnBloque(string query, int pos, ifstream& tIdxIn, string listaTerminosCompletos, ifstream& tLexico, int* docOffsetSiguiente);
	int obtenerOffsetDocsTerminosCompletos(ifstream& tIdxIn, int registro);
	int busquedaBinariaTerminos(string query, int cantRegistros, ifstream& tIdxIn, string listaTerminosCompletos);
private:
	IndexManager();
	static IndexManager* instance;
	int docOffset;
	int termOffset;
	int lexicOffset;
	string lastTerminoCompleto;
	string toString(int number);
	int cantPalabras;
	void saveTerminoCompleto(Termino* termino,ofstream& tIdx, ofstream& tLista);
	void saveLexico(Termino* termino, ofstream& tLexico);

};

#endif /* INDEXMANAGER_H_ */
