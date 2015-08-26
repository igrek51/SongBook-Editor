#include "io.h"
#include "config.h"
#include "system.h"
#include "files.h"
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
}


void IO::clear_log(){
    clear_file(Config::log_filename);
}

void IO::log(string l){
    fstream plik;
    plik.open(Config::log_filename.c_str(), fstream::out|fstream::app);
    if(!plik.good()){
        plik.close();
        message_box("B³¹d", "B³¹d zapisu do pliku dziennika: "+Config::log_filename);
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



	SetWindowText(hctrl[1],s.c_str());
	UpdateWindow(hwnd);



	log(s);
}

void IO::echo(int e){
    stringstream ss;
    ss<<e;
    echo(ss.str());
}



void App::get_argv(){
	string arg=GetCommandLine();
	if(arg.length()==0){
		argc=0;
		argv=NULL;
	}else{
		int arglen = arg.length();
		bool cudzyslow=false;
		//usuwanie podwójnych spacji
		for(int i=0; i<arglen-1; i++){
			if(arg[i]=='\"') cudzyslow=!cudzyslow;
			if(arg[i]==' '&&arg[i+1]==' '&&!cudzyslow){
				arg.erase(i,1);
				arglen--;
				i--;
			}
		}
		//usuwanie spacji z pocz¹tku
		if(arg[0]==' ') arg.erase(0,1);
		//usuwanie spacji z koñca
		if(arg[arg.length()-1]==' ') arg.erase(arg.length()-1,1);
		//policzenie parametrów
		cudzyslow=false;
		argc=1;
		for(unsigned int i=0; i<arg.length(); i++){
			if(arg[i]=='\"') cudzyslow=!cudzyslow;
			if(arg[i]==' '&&!cudzyslow) argc++;
		}
		argv = new string [argc];
		if(argc==1){
			argv[0]=arg;
		}else{
			int spaces=0;
			cudzyslow=0;
			for(int i=0; i<argc; i++) argv[i]="";
			for(unsigned int i=0; i<arg.length(); i++){
				if(arg[i]=='\"') cudzyslow=!cudzyslow;
				if(arg[i]==' '&&!cudzyslow){
					spaces++;
				}else{
					argv[spaces]+=arg[i];
				}
			}
		}
	}
	//odciêcie cudzys³owów
	for(int i=0; i<argc; i++){
		if(argv[i][0]=='\"'&&argv[i][argv[i].length()-1]=='\"'){
			argv[i] = argv[i].substr(1,argv[i].length()-2);
		}
	}
	ss_clear(ss);
	ss<<"Parametry ["<<argc<<"]: ";
	for(int i=0; i<argc; i++){
		ss<<argv[i];
		if(i<argc-1) ss<<"; ";
	}
	log(ss.str());
}

bool App::is_arg(string par){
	for(int i=1; i<argc; i++){
		if(argv[i]==par) return true;
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
