#include <iostream>
#include "Parser.h"
#include "Defines.h"
using namespace std;

// FALTA PONER ../ EN CADA UNO
string dir_repositorios = "Repositorios";
string file_repositorios = "Repositorios/repos.txt";
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

// Devuelve el termino de la lista de terminos completos dado el registro en el que se encuentra
string obtenerTermino(ifstream& tIdxIn, ifstream& tListaIn, int registro) {

	short cantCaracteres;
	int tOff;
	//Se posiciona en el registro
	tIdxIn.seekg(registro*TAM_TERMINO_REG,tIdxIn.beg);
	tIdxIn.read((char*)&cantCaracteres,2);
	tIdxIn.read((char*)&tOff,4);

	char* palabra = new char[cantCaracteres];
	tListaIn.seekg(tOff,tListaIn.beg);
	tListaIn.read(palabra,cantCaracteres);

	return palabra;


}

// Devuelve la posicion en el archivo de terminos completos donde puede llegar a estar la palabra buscada
int busquedaBinariaTerminos(string query, int cantRegistros, ifstream& tIdxIn, ifstream& tListaIn) {
	int der = cantRegistros - 1;
	int izq = 0;
	int medio;
	int res;
	bool encontrado = false;
	string palabra;
	while(izq <= der && !encontrado) {
		medio = (izq + der)/2;
		palabra = obtenerTermino(tIdxIn,tListaIn,medio);
		res = query.compare(palabra);
		// Encontro el termino
		if(res == 0) {
			encontrado = true;
		// Esta mas adelante
		} else if( res > 0) {
			izq = medio +1;
		//Esta mas atras
		} else {
			der = medio - 1;
		}
	}

	//Arreglo al metodo de busqueda binaria para que me devuelva el bloque en el que esta
	//Si no lo encuentra en el de terminos completos pero el ultimo resultado me dio mayor, devuelvo el anterior
	if(res < 0) {
		return medio - 1;
	}
	return medio;
}

//Devuelve el offset a documentos de la query o -1 si no lo encuentra
int busquedaEnBloque(string query, int pos, ifstream& tIdxIn, ifstream& tListaIn, ifstream& tLexico) {


	string terminoCompleto = obtenerTermino(tIdxIn,tListaIn,pos);
	int offLexico, offDocs;
	unsigned short iguales, distintos;

	string palabra;

	//PRIMERO ME FIJO SI ES EL DE TERMINOS COMPLETOS
	if(terminoCompleto.compare(query) == 0) {
		cout << "El termino " + query + " esta en la lista de completos" << endl;
	// BUSCO EN EL BLOQUE EN EL ARCHIVO DE LEXICO
	} else {
		int counter = 0;
		bool encontrado = false;
		tIdxIn.seekg(pos*TAM_TERMINO_REG+6,tIdxIn.beg); // +6 porque me salteo los dos primeros campos
		tIdxIn.read((char*)&offLexico,sizeof(offLexico));
		tLexico.seekg(offLexico,tLexico.beg);
		while(counter < kNFRONTCODING && !encontrado) {
			tLexico.read((char*)&iguales,sizeof(iguales));
			tLexico.read((char*)&distintos,sizeof(distintos));

			char* caracteresDistintos = new char[distintos];
			tLexico.read(caracteresDistintos,distintos);
			tLexico.read((char*)&offDocs,sizeof(offDocs));

			// Formo la palabra con los caracteres iguales del termino completo y los distintos extraidos del archivo de lexico
			palabra = terminoCompleto.substr(0,iguales) + caracteresDistintos;

			if(palabra.compare(query) == 0) {
				cout << "El termino " + query + " esta en el archivo de lexico" << endl;
				encontrado = true;
			}

			counter++;
		}
	}

	//TODO ir a buscar el offset de documentos
	return 0;

}

void consulta(string repo, string query) {
	// Abro todos los archivos
	ifstream tIdxIn(("Repositorios/" + repo + "/terminosIdx").c_str(),ios::binary|ios::in);
	ifstream tListaIn(("Repositorios/" + repo + "/terminosLista").c_str(),ios::binary|ios::in);
	ifstream tLexico(("Repositorios/" + repo + "/lexico").c_str(),ios::binary|ios::in);
	ifstream tDocs(("Repositorios/" + repo + "/documentos").c_str(),ios::binary|ios::in);

	// Calculo la cantidad total de registros
	tIdxIn.seekg(0,tIdxIn.end);
	int cantRegistros = tIdxIn.tellg()/TAM_TERMINO_REG;
	tIdxIn.seekg(0,tIdxIn.beg);

	int pos = busquedaBinariaTerminos(query,cantRegistros,tIdxIn,tListaIn);

	//Tengo que traerme el registro de documentos
	int docOffset = busquedaEnBloque(query,pos, tIdxIn, tListaIn, tLexico);

//TODO traerme el registro de documentos

	tIdxIn.close();
	tListaIn.close();

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
//	indexar("probando","texto_prueba");
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
	(			return 1;
			}
		}
*/
		//DESCOMENTAR ESTO Y COMENTAR EL OTRO
		//consulta(r,q);
		consulta("probando","carozo");
//
//	} else {
//		cout << "Instrucción inválida" << endl;
//		return 1;
//	}

	return 0;
}

