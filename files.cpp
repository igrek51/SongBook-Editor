#include "files.h"
#include "app.h"
#include <fstream>

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
