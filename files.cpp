#include "files.h"
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





void App::new_file(){
	SetWindowText(hctrl[2],"");
	refresh_text();
	opened_file="";
	update_title();
	echo("Nowy plik");
}

void App::open_file(string filename){
	if(!file_exists(filename)){
		ss_clear(ss);
		ss<<"! B³¹d: plik \""<<filename<<"\" nie istnieje";
		echo(ss.str());
		return;
	}
	fstream plik;
	plik.open(filename.c_str(),fstream::in|fstream::binary);
	if(!plik.good()){
		echo("! B³¹d otwarcia pliku");
		plik.close();
		return;
	}
	plik.seekg(0,plik.end);
	unsigned int fsize = plik.tellg();
	char *file_content = new char [fsize+1];
	file_content[fsize]=0;
	plik.seekg(0,plik.beg);
	plik.read(file_content,fsize);
	plik.close();
	SetWindowText(hctrl[2],file_content);
	set_selected(0,0,file_content);
	SendMessage(hctrl[2], EM_SCROLLCARET, 0, 0);
	opened_file = filename;
	ss_clear(ss);
	ss<<"Odczytano plik: "<<filename;
	echo(ss.str());
	SetWindowText(hctrl[0],filename.c_str());
	refresh_text();
	update_title();
	transposed=0;
	autoscroll_off();
}

void App::save_file(){
	char *new_filename = new char[256];
	GetWindowText(hctrl[0],new_filename,256);
	if(strlen(new_filename)==0){
		echo("Podaj nazwê pliku");
		delete[] new_filename;
		return;
	}
	opened_file = new_filename;
	delete[] new_filename;
	fstream plik;
	plik.open(opened_file.c_str(),fstream::out|fstream::binary);
	if(!plik.good()){
		echo("! B³¹d œcie¿ki pliku");
		plik.close();
		return;
	}
	char *str2 = load_text();
	plik.write(str2,str_size);
	plik.close();
	delete[] str2;
	update_title();
	ss_clear(ss);
	ss<<"Zapisano plik";
	if(transposed%12!=0){
		ss<<" (Zapis w nowej tonacji: ";
		if(transposed>0) ss<<"+";
		ss<<transposed;
		ss<<" !)";
	}
	echo(ss.str());
}

void App::save_file_line(string filename, string line){
    fstream plik;
    plik.open(filename.c_str(),fstream::out|fstream::binary);
    if(!plik.good()){
        echo("! B³¹d zapisu pliku");
        plik.close();
        return;
    }
    plik<<line;
    plik.close();
}

string App::read_file_line(string filename){
    string line="";
    if(!file_exists(filename)){
        ss_clear(ss);
        ss<<"! B³¹d: plik \""<<filename<<"\" nie istnieje";
        echo(ss.str());
        return "";
    }
    fstream plik;
    plik.open(filename.c_str(),fstream::in|fstream::binary);
    getline(plik,line);
    plik.close();
    return line;
}

