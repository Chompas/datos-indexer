#include <iostream>
#include "Parser.h"
using namespace std;

void indexar(string nombre, string dir) {

	//TODO: almacenamiento de repositorio
	Parser* parser = Parser::getInstance();
	parser->parsearDirectorio(dir);

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
		indexar(argv[2], argv[3]);

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
	}

	return 0;
}

