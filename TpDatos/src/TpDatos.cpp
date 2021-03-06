#include <iostream>
#include "Parser.h"
#include "Defines.h"
#include "FileManager.h"
#include "ByteBuffer.h"
#include "Termino.h"
#include "Utilidades.h"
#include "IndexManager.h"
#include "time.h"
#include <vector>
using namespace std;

// FALTA PONER ../ EN CADA UNO
string dir_repositorios = "../Repositorios";
string file_repositorios = "../Repositorios/repos.txt";
string file_paths = "/paths.txt";
string file_off = "/off.txt";

void indexar(string nombre, string dir) {

	//TODO: almacenamiento de repositorio

	mkdir(dir_repositorios.c_str(),S_IRWXU | S_IRWXG | S_IRWXO);

	//Crea carpeta para repositorio
	string repo_dir = dir_repositorios+"/"+nombre;
	mkdir(repo_dir.c_str() , S_IRWXU | S_IRWXG | S_IRWXO);

	//Agrega al archivo de repos
	ofstream repos;
	repos.open(file_repositorios.c_str(),ios::app | ios::binary);

	//Formato: num de bytes (short) + nombre (variable)
	short numBytes = nombre.length();
	repos.write((char*)&numBytes,sizeof(numBytes));
	repos << nombre;
	repos.close();

	//Creo archivo de documentos
	ofstream paths((repo_dir+file_paths).c_str(),ios::out | ios::binary);
	ofstream offsets((repo_dir+file_off).c_str(),ios::out | ios::binary);

	//Agrego el header de paths: numBytes + path raiz
	short numBytesRaiz = dir.length();
	paths.write((char*)&numBytesRaiz,sizeof(numBytesRaiz));
	paths << dir;

	Parser* parser = Parser::getInstance();
	parser->parsearDirectorio(dir,repo_dir,paths,offsets);

	paths.close();
	offsets.close();



}

string obtenerPathDocumento(int nro_doc, string repo) {
	string repo_dir = dir_repositorios+"/"+repo;
	ifstream paths((repo_dir+file_paths).c_str(),ios::in | ios::binary);
	ifstream offsets((repo_dir+file_off).c_str(),ios::in | ios::binary);
	string path;

	//Formato de offsets: nroDoc (short) + offset a paths (long)

	short nroOff;
	long offsetAPaths;

	offsets.seekg((nro_doc-1)* (sizeof(short)+sizeof(long)),offsets.beg);

	offsets.read((char*)&nroOff,sizeof(nroOff));
	offsets.read((char*)&offsetAPaths,sizeof(offsetAPaths));

	//Formato de paths: nro (short) + #bytes(short) + path restante (variable)

	short nroPaths;
	short numBytes;

	paths.seekg(offsetAPaths,paths.beg);

	paths.read((char*)&nroPaths,sizeof(nroPaths));
	paths.read((char*)&numBytes,sizeof(numBytes));
	char pathAux[numBytes];
	paths.read(pathAux,numBytes);
	path = pathAux;
	path = path.substr(0,numBytes);

	paths.close();
	offsets.close();

	return path;
}




void consulta(string repo, string query) {
	// Abro todos los archivos
	string dir_repo = dir_repositorios + "/";
	ifstream tIdxIn((dir_repo + repo + "/terminosIdx").c_str(),ios::binary|ios::in);
	ifstream tListaIn((dir_repo + repo + "/terminosLista").c_str(),ios::binary|ios::in);
	ifstream tLexico((dir_repo + repo + "/lexico").c_str(),ios::binary|ios::in);
	ifstream tDocs((dir_repo + repo + "/documentos").c_str(),ios::binary|ios::in);

	// Calculo la cantidad total de registros
	tIdxIn.seekg(0,tIdxIn.end);
	int cantRegistros = tIdxIn.tellg()/TAM_TERMINO_REG;
	tIdxIn.seekg(0,tIdxIn.beg);

	vector<Termino*> terminos;

	//Me traigo la lista entera de terminos completos para busqueda binaria

	tListaIn.seekg (0, tListaIn.end);
	int length = tListaIn.tellg();
	tListaIn.seekg (0, tListaIn.beg);

	char * bufferListaTerminosCompletos = new char [length];

	tListaIn.read (bufferListaTerminosCompletos,length);

	string listaTerminosCompletos = bufferListaTerminosCompletos;

	char* token = strtok((char*) query.c_str(), kSEPARADORES);
	if (token != NULL)
		Utilidades::string_a_minusculas(token);
	int posicion = 0;
	bool falla = false;
	while (token != NULL && !falla) {
		if (!Utilidades::isNumber(token) && strlen(token) > 1 && !Parser::getInstance()->isStopWord(token)) {//TODO: ESTA FEO PIDIENDOSELO AL PARSER: CAMBIAR

			int pos = IndexManager::getInstance()->busquedaBinariaTerminos(token,cantRegistros,tIdxIn,listaTerminosCompletos);

			//Tengo que traerme el registro de documentos
			int docOffsetSiguiente;
			int docOffset = IndexManager::getInstance()->busquedaEnBloque(token,pos, tIdxIn, listaTerminosCompletos, tLexico, &docOffsetSiguiente);

			string docRegister;
			if(docOffset >= 0){
				//Caso especial: que sea el ultimo
				if(docOffsetSiguiente < 0){
					tDocs.seekg (0, tDocs.end);
					docOffsetSiguiente = tDocs.tellg()*8;//Final del archivo de documentos
					tDocs.seekg (0, tDocs.beg);
				}
				docRegister = IndexManager::getInstance()->obtenerDocs(tDocs,docOffset,docOffsetSiguiente);
				terminos.push_back(IndexManager::getInstance()->decodeDocRegister(token, docRegister));
			} else {
				cout << "Palabra: " << token << endl;
				cout << "Consulta no encontrada" << endl;
				falla = true;
			}
		} else {
			terminos.push_back(NULL); //se salteo el termino
		}
		//Tomo el siguiente token
		token = strtok(NULL, kSEPARADORES);
		if (token != NULL)
			Utilidades::string_a_minusculas(token);
		posicion++;
	}

	if(!falla) {
		//Normalizacion de posiciones e interseccion de documentos
		vector<int> interseccionDocs;
		vector<Termino*>::const_iterator it;
		int counter = 0;
		for (it = terminos.begin(); it != terminos.end(); ++it) {
			if((*it)!=NULL) {

				if(interseccionDocs.empty()) {
					interseccionDocs = (*it)->docs;
				} else {
					IndexManager::getInstance()->intersecar(&interseccionDocs,(*it)->docs);
				}

				vector<long>* posicionesNormalizadas = new vector<long>;
				std::vector<vector<long> >::const_iterator positionListIt;
				for (positionListIt = (*it)->listaPosiciones.begin(); positionListIt != (*it)->listaPosiciones.end(); ++positionListIt){
					std::vector<long>::const_iterator positionIt;
					for(positionIt = positionListIt->begin(); positionIt != positionListIt->end() ; ++positionIt){
						posicionesNormalizadas->push_back((*positionIt)-counter);
						//cout << (*it)->palabra + ", " +
					}
					(*it)->listaPosicionesNormalizadas.push_back(*posicionesNormalizadas);
					posicionesNormalizadas->clear();
				}
			}
			counter++;
		}

		if(interseccionDocs.empty()) {
			cout << "No hay match" << endl;
		}

		vector<int>::const_iterator itInterseccion;
		vector<Termino*>::const_iterator itTerminos;
		bool salir, existeMatch = false;
		int position;
		vector<long> posicionesIntersecar;
		for(itInterseccion = interseccionDocs.begin(); itInterseccion != interseccionDocs.end(); ++itInterseccion) {
			posicionesIntersecar.clear();
			//cout << "Posible archivo: " << (*itInterseccion) << endl;
			bool noHayMatch = false;
			for (itTerminos = terminos.begin(); itTerminos != terminos.end() && !noHayMatch; ++itTerminos) {
				if((*itTerminos)!=NULL) {
					//cout << (*itTerminos)->palabra << endl;
					vector<int>::const_iterator itDoc;
					position = 0;
					salir = false;
					// Recorro documentos
					for(itDoc = (*itTerminos)->docs.begin(); itDoc!=(*itTerminos)->docs.end() && !salir; ++itDoc) {
						if(*itDoc == *itInterseccion) {
							salir = true;
						} else {
							position++;
						}
					}
					if(posicionesIntersecar.empty()) {
						posicionesIntersecar = (*itTerminos)->listaPosicionesNormalizadas[position];
					} else {
						IndexManager::getInstance()->intersecarPosiciones(&posicionesIntersecar,(*itTerminos)->listaPosicionesNormalizadas[position]);
						if(posicionesIntersecar.empty()) {
							noHayMatch = true;
						}
					}
				}
			}
			if(!posicionesIntersecar.empty()) {
				cout << "Match en el documento "<< *itInterseccion << ": " << obtenerPathDocumento(*itInterseccion,repo) << endl;
				existeMatch = true;
			}
//			} else {
//				cout << "No hay match" << endl;
//			}
		}
		if(!existeMatch) {
			cout << "No hay match" << endl;
		}
	}
	delete[] bufferListaTerminosCompletos;
	tIdxIn.close();
	tListaIn.close();
	tLexico.close();
	tDocs.close();

}


int main(int argc, char** argv) {

	if (argc < 2) {
		cout << "Pocos parámetros" << endl;
		return 1;
	}
	string instruccion = argv[1];

	if (instruccion == "i") {
		// Indexa
		if (argc != 4) {
			cout << "No se puede indexar" << endl;
			return 1;
		}
	//DESCOMENTAR ESTO Y COMENTAR EL OTRO
		indexar(argv[2], argv[3]);
//	indexar("probando","prueba");


	} else if (instruccion == "q") {
		// Consulta
		int c;
		char* r = NULL;
		char* q = NULL;

		while ((c = getopt(argc, argv, "r:q:")) != -1) {
			switch (c) {
			case 'r':
				r = optarg;
				break;
			case 'q':
				q = optarg;
				break;
			case '?':
				cout << "Instrucción inválida" << endl;
				return 1;
			default:
				return 1;
			}
		}

		//DESCOMENTAR ESTO Y COMENTAR EL OTRO
		consulta(r,q);
//	string query = "﻿Most people start at our Web site which has the main PG search facility:       http://www.gutenberg.org  This Web site includes information about Project Gutenberg-tm, including how to make donations to the Project Gutenberg Literary Archive Foundation, how to help produce our new eBooks, and how to subscribe to our email newsletter to hear about new eBooks.";
//	time_t start = time(0);
//	double seconds_since_start;
//		consulta("probando","casa");
//	seconds_since_start = difftime( time(0), start);
//	cout << seconds_since_start*1000 << endl;

	} else {
		cout << "Instrucción inválida" << endl;
		return 1;
	}

	return 0;
}

