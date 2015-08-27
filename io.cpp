#include "io.h"
#include "config.h"
#include "system.h"
#include "files.h"
#include "controls.h"
#include <fstream>
#include <ctime>

IO* IO::instance = NULL;

IO* IO::geti(){
    if(instance == NULL){
        instance = new IO();
    }
    return instance;
}

IO::IO(){
    last_echo = "";
    repeated_echo = 0;
    clear_log();
}


void IO::clear_log(){
    if(!Config::geti()->log_enabled) return;
    clear_file(Config::geti()->log_filename);
}

void IO::delete_log(){
    if(file_exists(Config::geti()->log_filename)){
        DeleteFile(Config::geti()->log_filename.c_str());
    }
}

void IO::log(string l){
    if(!Config::geti()->log_enabled) return;
    if(!file_exists(Config::geti()->log_filename)) clear_log();
    fstream plik;
    plik.open(Config::geti()->log_filename.c_str(), fstream::out|fstream::app);
    if(!plik.good()){
        plik.close();
        message_box("B³¹d", "B³¹d zapisu do pliku dziennika: "+Config::geti()->log_filename);
        return;
    }
    plik<<get_time()<<" - "<<l<<endl;
    plik.close();
}

void IO::log(int l){
    stringstream ss;
    ss<<l;
    log(ss.str());
}

void IO::log(string s, int l){
    stringstream ss;
    ss<<s<<": "<<l;
    log(ss.str());
}

void IO::error(string l, bool show_output){
    if(show_output){
        echo("[ B£¥D ! ] - "+l);
    }else{
        log("[ B£¥D ! ] - "+l);
    }
}

void IO::critical_error(string l){
    log("[ B£¥D KRYTYCZNY ! ] - "+l);
    message_box("B³¹d krytyczny", l);
}


void IO::echo(string s){
	//powtarzaj¹ce siê komunikaty
	if(s==last_echo){
		if(repeated_echo==0) repeated_echo=1;
		repeated_echo++;
	}else{
		repeated_echo=0;
	}
	last_echo = s;
	if(repeated_echo>0){
		stringstream ss;
		ss<<s<<" ("<<repeated_echo<<")";
		s = ss.str();
	}
    Controls::geti()->set_text("statusbar", s.c_str());
	log(s);
}

void IO::echo(int e){
    stringstream ss;
    ss<<e;
    echo(ss.str());
}


void IO::message_box(string title, string message){
    System::geti()->message_box(title, message);
}


void IO::get_args_from(string args_text){
    args.clear();
    //uciêcie spacji na koñcu
    if(args_text.length()>0){
        if(args_text[args_text.length()-1]==' '){
            args_text = args_text.substr(0, args_text.length()-1);
        }
    }
    bool cudzyslow = false;
    for(int i=0; i<(int)args_text.length(); i++){
        if(args_text[i]=='\"') cudzyslow = !cudzyslow;
        if(args_text[i]==' ' && !cudzyslow && i>0){
            args.push_back(args_text.substr(0, i));
            args_text = args_text.substr(i+1);
            i = -1;
        }
    }
    args.push_back(args_text);
    stringstream ss;
	ss<<"Parametry uruchomienia ("<<args.size()<<"): ";
	for(unsigned int i=0; i<args.size(); i++){
		ss<<args.at(i);
		if(i<args.size()-1) ss<<", ";
	}
	log(ss.str());
}

bool IO::is_arg(string parametr){
	for(unsigned int i=1; i<args.size(); i++){
		if(args.at(i)==parametr) return true;
	}
	return false;
}


void IO::set_workdir(){
    if(args.size()==0) return;
    int last_slash=0;
	for(int i=(int)args.at(0).length()-1; i>=0; i--){
        if(args.at(0)[i]=='\\' || args.at(0)[i]=='/'){
            last_slash = i;
            break;
        }
	}
	if(last_slash==0) return; //nie by³o slasha (lub by³ na pocz¹tku - LINUX!!!)
	string workdir = args.at(0).substr(0, last_slash);
	SetCurrentDirectory(workdir.c_str());
	log("Katalog roboczy: "+workdir);
}


string get_time(){
    time_t rawtime;
    struct tm *timeinfo;
    time(&rawtime);
    timeinfo = localtime(&rawtime);
    stringstream ss2;
    if(timeinfo->tm_hour<10) ss2<<"0";
    ss2<<timeinfo->tm_hour<<":";
    if(timeinfo->tm_min<10) ss2<<"0";
    ss2<<timeinfo->tm_min<<":";
    if(timeinfo->tm_sec<10) ss2<<"0";
    ss2<<timeinfo->tm_sec;
    return ss2.str();
}

void ss_clear(stringstream &sstream){
	sstream.str("");
	sstream.clear();
}
