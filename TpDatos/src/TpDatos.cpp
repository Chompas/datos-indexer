#include <iostream>
#include "Parser.h"
using namespace std;

string dir_repositorios = "../Repositorios";
string file_repositorios = "../Repositorios/repos.txt";
string file_paths = "/paths.txt";
string file_off = "/off.txt";

void indexar(string nombre, string dir) {

	//TODO: almacenamiento de repositorio

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
	ofstream paths((repo_dir+file_paths).c_str(),ios::app | ios::binary);
	ofstream offsets((repo_dir+file_off).c_str(),ios::app | ios::binary);

	//Agrego el header de paths: numBytes + path raiz
	short numBytesRaiz = dir.length();
	paths.write((char*)&numBytesRaiz,sizeof(numBytesRaiz));
	paths << dir;

	Parser* parser = Parser::getInstance();
	parser->parsearDirectorio(dir,repo_dir,paths,offsets);

	paths.close();
	offsets.close();



}

int main(int argc, char** argv) {

	/*if (argc < 2) {
		cout << "Pocos parámetros" << endl;
		return 1;
	}
	string instruccion = argv[1];

	if (instruccion == "i") {
		// Indexa
		if (argc != 4) {
			cout << "No se puede indexar" << endl;
			return 1;
		}*/
	//DESCOMENTAR ESTO Y COMENTAR EL OTRO
	//	indexar(argv[2], argv[3]);
	indexar("sadasda","textos");
/*
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

		//consulta(r,q);

	} else {
		cout << "Instrucción inválida" << endl;
		return 1;
	}*/

	return 0;
}

