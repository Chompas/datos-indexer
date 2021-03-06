/*
 * Parser.cpp
 *
 *  Created on: 01/05/2013
 *      Author: lara
 */

#include "Parser.h"
#include <sstream>
#include <algorithm>
#include "IndexManager.h"
#include "ByteBuffer.h"
#include "Defines.h"
#include "Utilidades.h"
#include "FileManager.h"

int MAX_MEM = 1024;

Parser* Parser::instance = 0;

Parser::Parser() {

	//Cargo las stop words en memoria
	ifstream sWords("../stopwords.txt");
	string stop_word;
	while(sWords.good()) {
		getline(sWords,stop_word,',');
		stopWords.push_back(stop_word);
	}
	sWords.close();

}

Parser* Parser::getInstance() {
	if (instance == 0) {
		instance = new Parser();
	}
	return instance;
}

// Devuelve la posicion
int busquedaBinaria(string texto, vector<string>* terminos) {
	int der = terminos->size() - 1;
	int izq = 0;
	int medio = -1;
	int res;
	bool encontrado = false;
	string palabra;
	while(izq <= der && !encontrado) {
		medio = (izq + der)/2;
		palabra = terminos->at(medio);
		res = texto.compare(palabra);
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

	if(!encontrado) medio = -1;
	return medio;
}

bool Parser::isStopWord(char* word) {
	int pos = busquedaBinaria(word,&stopWords);
	return pos>=0;
}

/*
 * Recorre el archivo y va separando en tokens cada linea. Asigna los terminos al vector de terminos
 */
void Parser::processFile(const char* path, short nro_doc,
		vector<TerminoRegister>* terminos, int* memoriaUsada) {
	string line;
	ifstream fin;
	fin.open(path);
	long posicion = 1;
	while (!fin.eof()) {

		getline(fin, line);
		char* token = strtok((char*) line.c_str(), kSEPARADORES);
		if (token != NULL) {
			Utilidades::string_a_minusculas(token);
		}
		while (token != NULL) {
			if (!Utilidades::isNumber(token) && strlen(token) > 1 && !isStopWord(token)) {

				TerminoRegister termino;
				termino.setDocumento(nro_doc);
				termino.setTermino(token);
				termino.setPosicion(posicion);
				terminos->push_back(termino);
				(*memoriaUsada)++;
			}
			//Tomo el siguiente token

			token = strtok(NULL, kSEPARADORES);
			if (token != NULL) {
				Utilidades::string_a_minusculas(token);
			}
			posicion++;
		}
	}
	fin.close();
}

void imprimirArchivoParcial(vector<TerminoRegister> terminos) {
	cout << endl << "*****************Nuevo Doc Auxiliar*****************"
			<< endl;
	cout << "Termino-Documento-Frecuencia-Posiciones" << endl;
	for (vector<TerminoRegister>::iterator it = terminos.begin();
			it != terminos.end(); ++it) {

		cout << it->getTermino() << " - " << it->getDocumento() << " - "
				<< it->getFrecuencia() << " - ";
		for (vector<long>::iterator pos = it->getPosiciones().begin();
				pos != it->getPosiciones().end(); ++pos) {
			cout << *pos << " ";
		}
		cout << endl;
	}
}

void Parser::procesarTerminos(ifstream& file,ofstream& tIdxOut,ofstream& tListaOut,ofstream& tLexicoOut,ofstream& tDocsOut) {
	TerminoRegister termino = FileManager::leerTermino(file);
	Termino* nuevoTermino = new Termino(termino);
	string palabra = termino.getTermino();
	while(!file.eof()) {
		nuevoTermino->addPositionsForDoc(termino.getDocumento(),termino.getPosicion());
		termino = FileManager::leerTermino(file);
		//Si es un nuevo termino
		if( termino.getTermino().compare(palabra) != 0) {
			IndexManager::getInstance()->indexTerm(nuevoTermino,tIdxOut,tListaOut,tLexicoOut,tDocsOut);
			delete nuevoTermino;
			nuevoTermino = new Termino(termino);
			palabra = termino.getTermino();
		}

	}
}

typedef struct file {

	string path;
	string name;
	int nro;
	long size;

}file_t;

long fileSize(string filepath) {
	ifstream in;
	in.open(filepath.c_str(), ios::in | ios::binary);
	in.seekg(0, std::ifstream::end);
	long size = in.tellg();
	in.close();
	return size;
}

bool file_cmp(file_t f1, file_t f2) {
	return f1.size < f2.size;
}

bool temp_file_cmp(file_t f1, file_t f2) {
	return f1.size > f2.size;
}

file_t tempMerge(file_t f1, file_t f2, string filepath) {
	ifstream f1In(f1.path.c_str(),ios::in|ios::binary);
	ifstream f2In(f2.path.c_str(),ios::in|ios::binary);

	TerminoRegister termino1 = FileManager::leerTermino(f1In);
	TerminoRegister termino2 = FileManager::leerTermino(f2In);

	ofstream newFile(filepath.c_str(),ios::out|ios::binary);
	bool res;

	while(!f1In.eof() && !f2In.eof()) {
		res = TerminoRegister::cmp(termino1,termino2);
		if(res) {
			FileManager::guardarStreamTemporal(termino1,newFile);
			termino1 = FileManager::leerTermino(f1In);
		} else {
			FileManager::guardarStreamTemporal(termino2,newFile);
			termino2 = FileManager::leerTermino(f2In);
		}
	}

	if(f1In.eof()) {
		while(!f2In.eof()) {
			FileManager::guardarStreamTemporal(termino2,newFile);
			termino2 = FileManager::leerTermino(f2In);
		}
	} else {
		while(!f1In.eof()) {
			FileManager::guardarStreamTemporal(termino1,newFile);
			termino1 = FileManager::leerTermino(f1In);
		}
	}

	f1In.close();
	f2In.close();
	newFile.close();

	file_t newTemp;
	newTemp.path = filepath;
	newTemp.size = fileSize(filepath);

	return newTemp;

}

/*
 * Recorre el directorio y procesa archivo por archivo del mismo
 */
void Parser::recorrerDirectorio(string dir, ofstream &paths, ofstream &offsets) {
	time_t start = time(0);
	ofstream docFile;
	string filepath;
	DIR *dp;
	struct dirent *dirp;
	struct stat filestat;

	vector<TerminoRegister> terminos;

	dp = opendir(dir.c_str());

	if (dp == NULL) {
		cout << "Directorio inválido" << endl;
		return;
	}

	short nro_doc = 0;
	int memoriaUsada = 0;
	//Offset inicial: despues del header
	long offset = sizeof(short)+dir.length();

	double seconds_since_start;

	vector<file_t> files;
	vector<file_t> tempFiles;

	while ((dirp = readdir(dp))) {
		filepath = dir + "/" + dirp->d_name;

		// Saltea subdirectorios
		if (stat(filepath.c_str(), &filestat)) {
			continue;
		}
		if (S_ISDIR(filestat.st_mode)) {
			continue;
		}

		//Identificacion de los documentos a indexar: guarda y numera documentos
		nro_doc++;

		file_t file;
		file.path = filepath;
		file.name = dirp->d_name;
		file.size = fileSize(filepath);
		files.push_back(file);
	}

	sort(files.begin(),files.end(),file_cmp);

	for(unsigned int i = 0; i< files.size(); i++) {
		//TODO: comprobar si se lleno la memoria, de ser asi, bajo a disco
	//	if (memoriaUsada >= MAX_MEM) {
		files[i].nro = i+1;
		guardarDocumento(files[i].name,files[i].nro,paths,offsets,&offset);
		cout << files[i].nro << ") Procesando " + files[i].name << endl;
		this->processFile(files[i].path.c_str(), files[i].nro, &terminos, &memoriaUsada);

		//Ordeno la lista de terminos
		sort(terminos.begin(),terminos.end(),TerminoRegister::cmp);

		// TRAERME TERMINOS DE DISCO, JUNTARLOS CON LOS ACTUALES
		//vector<TerminoRegister> terminosFinal = procesarTerminos(&terminos);

		FileManager::guardarArchivoTemporal(terminos,repo_dir,i);
		file_t temp;
		temp.name = Utilidades::toString(i);
		temp.nro = i;
		temp.path = repo_dir + "/" + temp.name;
		temp.size = fileSize(temp.path);

		tempFiles.push_back(temp);

		//imprimirArchivoParcial(terminos);
		//guardarEnDisco(terminosFinal);
		terminos.clear();
	//	}
	}

	file_t temp1;
	file_t temp2;

	int counter = tempFiles.size();

	while(tempFiles.size() > 1) {
		sort(tempFiles.begin(),tempFiles.end(),temp_file_cmp);
		temp1 = tempFiles.back();
		tempFiles.pop_back();
		temp2 = tempFiles.back();
		tempFiles.pop_back();
		cout << "**********MERGING "+ temp1.path + " - " + temp2.path + " en " << counter << " ********************"<< endl;
		tempFiles.push_back(tempMerge(temp1,temp2,repo_dir + "/" + Utilidades::toString(counter)));
//		cout << " FIN DE MERGE" << endl;
		remove(temp1.path.c_str());
		remove(temp2.path.c_str());
		counter++;
	}

	ifstream archivoFinal(tempFiles.front().path.c_str(),ios::in|ios::binary);

	ofstream tIdxOut((repo_dir + "/terminosIdx").c_str(),ios::binary|ios::out);
	ofstream tListaOut((repo_dir + "/terminosLista").c_str(),ios::binary|ios::out);
	ofstream tLexicoOut((repo_dir + "/lexico").c_str(),ios::binary|ios::out);
	ofstream tDocsOut(( repo_dir + "/documentos").c_str(),ios::binary|ios::out);

	cout << "************INDEXANDO Y GUARDANDO EN DISCO*******************" << endl;

	procesarTerminos(archivoFinal,tIdxOut,tListaOut,tLexicoOut,tDocsOut);


	//VACIO EL BUFFER DE DOCUMENTOS
	ByteBuffer::getInstance()->vaciar(tDocsOut);
	IndexManager::getInstance()->reset();

	terminos.clear();

	cout << "************GUARDADO EN DISCO*******************" << endl;

	tIdxOut.close();
	tListaOut.close();
	tLexicoOut.close();
	tDocsOut.close();

	archivoFinal.close();
	remove(tempFiles.front().path.c_str());


	//guardarEnDisco(terminosFinal);


//	seconds_since_start = difftime( time(0), start);
//	cout << seconds_since_start/60 << endl;
//

	// PARA EL ULTIMO
//	//Ordeno la lista de terminos
//	sort(terminos.begin(),terminos.end(),TerminoRegister::cmp);
//
//	vector<TerminoRegister> terminosFinal = procesarTerminos(&terminos);
//	//imprimirArchivoParcial(terminos);
//	guardarEnDisco(terminosFinal);

	closedir(dp);
}

vector<Termino*> levantarIndice(ifstream& tIdxIn,ifstream& tListaIn,ifstream& tLexicoIn,ifstream& tDocsIn) {

	cout << "************LEVANTANDO INDICE*******************" << endl;

	vector<Termino*> terminos;
	//Me traigo la lista entera de terminos completos para busqueda binaria

	tListaIn.seekg (0, tListaIn.end);
	int length = tListaIn.tellg();
	tListaIn.seekg (0, tListaIn.beg);

	if(length < 0) {
		return terminos;
	}

	char * bufferListaTerminosCompletos = new char [length];

	tListaIn.read (bufferListaTerminosCompletos,length);

	string listaTerminosCompletos = bufferListaTerminosCompletos;

	int offLexico,docOffset,docOffsetSiguiente,pos = 0;


	tIdxIn.seekg (0, tIdxIn.end);
	int largoIdx = tIdxIn.tellg();
	tIdxIn.seekg (0, tIdxIn.beg);

	int max_pos = largoIdx/14;// 14 por el largo de cada registro

	while(pos<max_pos) {

		//AGARRO TERMINO COMPLETO
		//RECORRO BLOQUE DECODIFICANDO
		//GUARDO EN EL VECTOR DE TERMINOS


		string terminoCompleto = IndexManager::getInstance()->obtenerTermino(tIdxIn,listaTerminosCompletos,pos);
		if(terminoCompleto.empty()) {
			break; // TODO: MEJORAR TODO ESTOOOOOOO
		}
		//tIdxIn.seekg(pos*TAM_TERMINO_REG+6,tIdxIn.beg); // +6 porque me salteo los dos primeros campos
		tIdxIn.read((char*)&offLexico,sizeof(offLexico));
		//tLexicoIn.seekg(offLexico,tLexicoIn.beg);

		//Tengo que traerme el registro de documentos
		docOffset = IndexManager::getInstance()->busquedaEnBloque(terminoCompleto,pos, tIdxIn, listaTerminosCompletos, tLexicoIn, &docOffsetSiguiente);

		string docRegister;
		if(docOffset >= 0){
			//Caso especial: que sea el ultimo
			if(docOffsetSiguiente < 0){
				tDocsIn.seekg (0, tDocsIn.end);
				docOffsetSiguiente = tDocsIn.tellg()*8;//Final del archivo de documentos
				tDocsIn.seekg (0, tDocsIn.beg);
			}
			docRegister = IndexManager::getInstance()->obtenerDocs(tDocsIn,docOffset,docOffsetSiguiente);
			terminos.push_back(IndexManager::getInstance()->decodeDocRegister(terminoCompleto, docRegister));
		}

		unsigned short iguales, distintos;
		string palabra;
		tLexicoIn.seekg(offLexico,tLexicoIn.beg);
		for(int i = 0; i < kNFRONTCODING -1 && !tLexicoIn.eof(); i++) {

			tLexicoIn.read((char*)&iguales,sizeof(iguales));
			tLexicoIn.read((char*)&distintos,sizeof(distintos));

			char* caracteresDistintosAux = new char[distintos];
			tLexicoIn.read(caracteresDistintosAux,distintos);
			tLexicoIn.read((char*)&docOffset,sizeof(docOffset));
			string caracteresDistintos = caracteresDistintosAux;
			delete caracteresDistintosAux;
			caracteresDistintos = caracteresDistintos.substr(0,distintos);

			// Formo la palabra con los caracteres iguales del termino completo y los distintos extraidos del archivo de lexico
			palabra = terminoCompleto.substr(0,iguales) + caracteresDistintos;
			int offLexicoActual;
			if(i == kNFRONTCODING -1) {
				docOffsetSiguiente = IndexManager::getInstance()->obtenerOffsetDocsTerminosCompletos(tIdxIn,pos+1);
			}else{
				offLexicoActual = tLexicoIn.tellg();
				tLexicoIn.read((char*)&iguales,sizeof(iguales));
				tLexicoIn.read((char*)&distintos,sizeof(distintos));

				char* caracteresDistintos = new char[distintos];
				tLexicoIn.read(caracteresDistintos,distintos);
				delete caracteresDistintos;
				tLexicoIn.read((char*)&docOffsetSiguiente,sizeof(docOffsetSiguiente));
				//Caso especial: que sea el ultimo
				if(tLexicoIn.eof()){
					 docOffsetSiguiente = -1;
				}
			}
			if(docOffset >= 0){
				//Caso especial: que sea el ultimo
				if(docOffsetSiguiente < 0){
					tDocsIn.seekg (0, tDocsIn.end);
					docOffsetSiguiente = tDocsIn.tellg()*8;//Final del archivo de documentos
					tDocsIn.seekg (0, tDocsIn.beg);
				}
				docRegister = IndexManager::getInstance()->obtenerDocs(tDocsIn,docOffset,docOffsetSiguiente);
				terminos.push_back(IndexManager::getInstance()->decodeDocRegister(palabra, docRegister));
			}
			if(!tLexicoIn.eof()) {
				tLexicoIn.seekg(offLexicoActual,tLexicoIn.beg);
			}

		}

		pos++;
	}

	delete bufferListaTerminosCompletos;
	return terminos;
}

void rellenar(vector<Termino*>* final, vector<Termino*> terminos, size_t pos) {
	for(size_t i = pos ; i<terminos.size(); i++) {
		final->push_back(terminos[i]);
	}
}

vector<Termino*> merge(vector<Termino*> terminosActuales, vector<Termino*> terminosIndice) {

	cout << "************MERGING*******************" << endl;

	vector<Termino*> final;

	size_t posActuales = 0, posIndice = 0;
	int res;
	Termino* nuevoTermino;

	while(posActuales < terminosActuales.size() && posIndice < terminosIndice.size()) {
		res = strcmp(terminosActuales[posActuales]->palabra.c_str(),terminosIndice[posIndice]->palabra.c_str());
		if(res < 0) {
			final.push_back(terminosActuales[posActuales]);
			posActuales++;
		}else if(res > 0) {
			final.push_back(terminosIndice[posIndice]);
			posIndice++;
		} else {
			nuevoTermino = terminosIndice[posIndice];
			//nuevoTermino->addPositionsForDoc(terminosActuales[posActuales]->docs[0],terminosActuales[posActuales]->listaPosiciones[0]);
			delete terminosActuales[posActuales];
			final.push_back(nuevoTermino);
			posActuales++;
			posIndice++;
		}
	}

	if(posActuales == terminosActuales.size()) {
		rellenar(&final,terminosIndice,posIndice);
	} else {
		rellenar(&final,terminosActuales,posActuales);
	}

	return final;
}

void Parser::guardarEnDisco(vector<Termino*> terminos){

	//ACA VA EL MERGE


	cout << "************TERMINOS ACTUALES EN MEMORIA*******************" << endl;

	//PROCESAR ULTIMO
	//IndexManager::getInstance()->indexTerm(termino,tIdx,tLista,tLexicoOut,tDocsOut);


//	ifstream tIdxIn((repo_dir + "/terminosIdx").c_str(),ios::binary|ios::in);
//	ifstream tListaIn((repo_dir + "/terminosLista").c_str(),ios::binary|ios::in);
//	ifstream tLexicoIn((repo_dir + "/lexico").c_str(),ios::binary|ios::in);
//	ifstream tDocsIn(( repo_dir + "/documentos").c_str(),ios::binary|ios::in);
//
//
//
//	//vector<Termino*> terminosFinales = merge(terminosActuales,levantarIndice(tIdxIn,tListaIn,tLexicoIn,tDocsIn));
//
//	tIdxIn.close();
//	tListaIn.close();
//	tLexicoIn.close();
//	tDocsIn.close();


	ofstream tIdxOut((repo_dir + "/terminosIdx").c_str(),ios::binary|ios::out);
	ofstream tListaOut((repo_dir + "/terminosLista").c_str(),ios::binary|ios::out);
	ofstream tLexicoOut((repo_dir + "/lexico").c_str(),ios::binary|ios::out);
	ofstream tDocsOut(( repo_dir + "/documentos").c_str(),ios::binary|ios::out);

	cout << "************INDEXANDO Y GUARDANDO EN DISCO*******************" << endl;

	vector<Termino*>::const_iterator it;
	for(it = terminos.begin(); it!=terminos.end(); ++it) {
		IndexManager::getInstance()->indexTerm(*it,tIdxOut,tListaOut,tLexicoOut,tDocsOut);
		delete *it;
	}


	//VACIO EL BUFFER DE DOCUMENTOS
	ByteBuffer::getInstance()->vaciar(tDocsOut);
	IndexManager::getInstance()->reset();

	terminos.clear();

	cout << "************GUARDADO EN DISCO*******************" << endl;

	tIdxOut.close();
	tListaOut.close();
	tLexicoOut.close();
	tDocsOut.close();


}

void Parser::parsearDirectorio(string dir, string repo_dir, ofstream &paths, ofstream &offsets) {
	this->repo_dir = repo_dir;
	this->recorrerDirectorio(dir,paths,offsets);
}

void Parser::guardarDocumento(string filepath, short nro_doc, ofstream &paths, ofstream &offsets, long* offset) {
	//Formato de paths: nro (short) + #bytes(short) + path restante (variable)
	paths.write((char*)&nro_doc,sizeof(nro_doc));
	short numBytes = filepath.length();
	paths.write((char*)&numBytes,sizeof(numBytes));
	paths << filepath;

	//Formato de offsets: nroDoc (short) + offset a paths (long)
	offsets.write((char*)&nro_doc,sizeof(nro_doc));
	offsets.write((char*)offset,sizeof(*offset));
	//Se le suma al offset lo agregado en el archivo de paths
	(*offset)+=sizeof(nro_doc)+sizeof(numBytes)+filepath.length();
}


