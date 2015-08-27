#include "io.h"
#include "config.h"
#include "system.h"
#include "files.h"
#include "controls.h"
#include <fstream>
#include <sstream>
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
    clear_log();
    System::geti()->get_args();
}


void IO::clear_log(){
    clear_file(Config::geti()->log_filename);
}

void IO::log(string l){
    if(!Config::geti()->log_enabled) return;
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

void IO::error(string l){
    echo("[ B£¥D ! ] - "+l);
}

void IO::critical_error(string l){
    echo("[ B£¥D KRYTYCZNY ! ] - "+l);
    message_box("B³¹d", l);
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
		ss_clear(ss);
		ss<<s<<" ("<<repeated_echo<<")";
		s=ss.str();
	}
    System::geti()->set_text("statusbar", s.c_str());
	log(s);
}

void IO::echo(int e){
    stringstream ss;
    ss<<e;
    echo(ss.str());
}


void IO::get_args_from(string args_text){
    args.clear();
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
	for(int i=0; i<args.size(); i++){
		ss<<args.at(i);
		if(i<args.size()-1) ss<<", ";
	}
	log(ss.str());
}

bool IO::is_arg(string parametr){
	for(int i=1; i<args.size(); i++){
		if(args.at(i)==parametr) return true;
	}
	return false;
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
