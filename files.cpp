#include "files.h"
#include "io.h"
#include <fstream>
#include <windows.h>

void clear_file(string filename){
	fstream plik;
	plik.open(filename.c_str(),fstream::out|fstream::trunc);
	plik.close();
}

bool file_exists(string name){
	ifstream plik(name.c_str());
	if(plik.good()){
		plik.close();
		return true;
	}else{
		plik.close();
		return false;
	}
}

bool dir_exists(string name){
    DWORD ftyp = GetFileAttributesA(name.c_str());
    if(ftyp==INVALID_FILE_ATTRIBUTES) return false;
    if(ftyp&FILE_ATTRIBUTE_DIRECTORY) return true;
    return false;
}

vector<string>* get_all_lines(string filename){
    fstream plik;
    plik.open(filename.c_str(),fstream::in|fstream::binary);
    if(!plik.good()){
        plik.close();
        return NULL;
    }
    vector<string>* lines = new vector<string>;
    string linia;
    do{
        getline(plik,linia,'\n');
        //usuniêcie znaków \r
        for(unsigned int i=0; i<linia.length(); i++){
            if(linia[i]=='\r'){
                linia.erase(linia.begin()+i);
                i--;
            }
        }
        lines->push_back(linia);
    }while(!plik.eof());
    plik.close();
    return lines;
}


char* open_file(string filename){
    if(!file_exists(filename)){
		IO::geti()->error("plik \""+filename+"\" nie istnieje");
		return NULL;
	}
	fstream plik;
	plik.open(filename.c_str(), fstream::in|fstream::binary);
	if(!plik.good()){
		IO::geti()->error("B³¹d otwarcia pliku");
		plik.close();
		return NULL;
	}
	plik.seekg(0, plik.end);
	unsigned int fsize = plik.tellg();
	char *file_content = new char [fsize+1];
	file_content[fsize] = 0;
	plik.seekg(0, plik.beg);
	plik.read(file_content, fsize);
	plik.close();
    return file_content;
}

bool save_file(string filename, string content){
    fstream plik;
	plik.open(filename.c_str(), fstream::out|fstream::binary);
	if(!plik.good()){
		IO::geti()->error("B³¹d œcie¿ki pliku");
		plik.close();
		return false;
	}
	plik.write(content.c_str(), content.length());
	plik.close();
    return true;
}
