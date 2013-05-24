/*
 * FileManager.h
 *
 *  Created on: 24/05/2013
 *      Author: lara
 */

#ifndef FILEMANAGER_H_
#define FILEMANAGER_H_

#include <string>
using namespace std;

class FileManager {
public:
	static FileManager* getInstance();
	virtual ~FileManager();
	static int saveToFile(string buffer,string filepath );
private:
	static FileManager* instance;
	FileManager();
};

#endif /* FILEMANAGER_H_ */
