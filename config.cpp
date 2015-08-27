#include "config.h"
#include "files.h"
#include "io.h"
#include <cstdlib>

Config* Config::instance = NULL;

Config* Config::geti(){
    if(instance == NULL){
        instance = new Config();
    }
    return instance;
}

Config::Config(){
    //Sta³e
    config_filename = "conf.ini";
    log_filename = "log.txt";
    buttons_fontface = "MS Shell Dlg 2";
    program_name = "Igrek SongBook Editor";
    //Zmienne
    window_w = 600;
    window_h = 680;
    control_height = 22;
    transposed = 0;
	autoscroll = false;
    autoscroll_count = 0;
    fullscreen_on = false;
	opened_file = "";
    file_to_open = "";
	for(int i=0; i<9; i++) tekst_wstaw[i]="";
    //wczytywane z pliku - wartoœci domyœlne
    songs_dir = ".";
    editor_fontface = "Calibri";
	halfscreen = false;
    log_enabled = true;
    toolbar_show = true;
    autoscroll_scaling = false;
    chordsbase_on_startup = false;
    buttons_fontsize = 18;
    editor_fontsize = 18;
    autoscroll_interval = 500;
	autoscroll_wait = 35;
}

ConfigVariable::ConfigVariable(string name, string value){
    this->name = name;
    this->value = value;
}


void Config::load_from_file(){
    if(!file_exists(config_filename)){
        IO::geti()->error("Brak pliku konfiguracyjnego - ³adowanie wartoœci domyœlnych");
        return;
    }
    vector<ConfigVariable*>* variables = get_config_variables(config_filename);
    //odczyt zmiennych
	songs_dir = get_config_string(variables, "songs_dir");
	editor_fontface = get_config_string(variables, "editor_fontface");
	autoscroll_interval = get_config_int(variables, "autoscroll_interval");
	autoscroll_wait = get_config_int(variables, "autoscroll_wait");
	autoscroll_scaling = get_config_bool(variables, "autoscroll_scaling");
	halfscreen = get_config_bool(variables, "halfscreen");
    buttons_fontsize = get_config_int(variables, "buttons_fontsize");
    editor_fontsize = get_config_int(variables, "editor_fontsize");
    log_enabled = get_config_bool(variables, "log_enabled");
    toolbar_show = get_config_bool(variables, "toolbar_show");
    chordsbase_on_startup = get_config_bool(variables, "chordsbase_on_startup");
    //sprz¹tanie
    for(unsigned int i=0; i<variables->size(); i++){
        delete variables->at(i);
    }
    variables->clear();
    IO::geti()->log("Wczytano ustawienia z pliku: "+config_filename);
}

vector<string>* Config::get_config_lines(string filename){
    vector<string>* lines = get_all_lines(filename);
    if(lines==NULL) return NULL;
    for(int i=0; i<(int)lines->size(); i++){
        if(lines->at(i).length()==0){ //usuniêcie pustych elementów
            lines->erase(lines->begin()+i);
            i--;
            continue;
        }
        if(lines->at(i).length()>=2){ //usuniêcie komentarzy
            if(lines->at(i)[0]==';' || (lines->at(i)[0]=='/' && lines->at(i)[1]=='/')){
                lines->erase(lines->begin()+i);
                i--;
            }
        }
    }
    return lines;
}

vector<ConfigVariable*>* Config::get_config_variables(string filename){
    vector<string>* lines = get_config_lines(filename);
    if(lines==NULL) return NULL;
    vector<ConfigVariable*>* variables = new vector<ConfigVariable*>;
    //wy³uskanie nazwy i wartoœci
    for(unsigned int i=0; i<lines->size(); i++){
        //szukanie znaku =
        for(unsigned int j=1; j<lines->at(i).length(); j++){
            if(lines->at(i)[j] == '='){
                string name = lines->at(i).substr(0, j);
                string value = lines->at(i).substr(j+1);
                //obciêcie spacji na pocz¹tku i na koñcu
                if(name.length()>0 && name[name.length()-1] == ' '){
                    name = name.substr(0, name.length()-1);
                }
                if(value.length()>0 && value[0] == ' '){
                    value = value.substr(1);
                }
                variables->push_back(new ConfigVariable(name, value));
                break;
            }
        }
    }
    lines->clear();
    return variables;
}

string Config::get_config_string(vector<ConfigVariable*>* variables, string name){
    if(variables==NULL) return "";
    for(unsigned int i=0; i<variables->size(); i++){
        if(variables->at(i)->name == name){
            return variables->at(i)->value;
        }
    }
    IO::geti()->error("Nie znaleziono zmiennej w pliku konfiguracyjnym: "+name);
    return "";
}

int Config::get_config_int(vector<ConfigVariable*>* variables, string name){
    string s = get_config_string(variables, name);
    if(s.length()==0) return 0;
    return atoi(s.c_str());
}

bool Config::get_config_bool(vector<ConfigVariable*>* variables, string name){
    string s = get_config_string(variables, name);
    if(s.length()==0) return false;
    if(s=="true") return true;
    if(s=="1") return true;
    return false;
}
