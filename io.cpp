#include "app.h"
#include <fstream>
#include <ctime>

string get_time_date(){
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
  ss2<<timeinfo->tm_sec<<", ";
  if(timeinfo->tm_mday<10) ss2<<"0";
  ss2<<timeinfo->tm_mday<<".";
  if(timeinfo->tm_mon+1<10) ss2<<"0";
  ss2<<timeinfo->tm_mon+1<<".";
  ss2<<timeinfo->tm_year+1900;
  return ss2.str();
}

void App::log(string l){
	if(config_log_enabled!=1) return;
	fstream plik;
	plik.open("log.txt",fstream::out|fstream::app);
	plik<<get_time_date()<<" - "<<l<<endl;
	plik.close();
}

void App::echo(string s){
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

void App::echo(int e){
	ss_clear(ss);
	ss<<e;
	echo(ss.str());
}

void App::ss_clear(stringstream &sstream){
	sstream.str("");
	sstream.clear();
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

void App::message(string m){
	MessageBox(hwnd,m.c_str(),"Info",MB_OK|MB_ICONINFORMATION);
}
