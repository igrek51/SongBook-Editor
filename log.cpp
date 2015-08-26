#include "config.h"
#include "files.h"

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
    //Zmienne
    songs_dir = ".";
	config_halfscreen = 0;
    config_fontface = "Calibri";
    config_fontsize1 = 18;
    config_fontsize2 = 18;
    config_log_enabled = 1;
    config_pasek_enabled = 1;
    config_autoscroll_scale = 0;
    config_chordsbase_on_startup = 0;
    autoscroll_interval = 500;
	autoscroll_wait = 35;
}

ConfigVariable::ConfigVariable(string name, string value){
    this->name = name;
    this->value = value;
}


void Config::load_from_file(){
    Log::ok("Wczytywanie ustawieñ");
    if(!file_exists(config_filename)){
        Log::error("Brak pliku konfiguracyjnego");
        return;
    }
    vector<ConfigVariable*>* variables = get_config_variables(config_filename);
    //odczyt zmiennych
	songs_dir = get_config_string(variables, "songs_dir");
	config_fontface = get_config_string(variables, "fontface");
	autoscroll_interval = get_config_int(variables, "autoscroll_interval");
	autoscroll_wait = get_config_int(variables, "autoscroll_wait");
	config_autoscroll_scale = get_config_int(variables, "autoscroll_scale");
	config_halfscreen = get_config_int(variables, "halfscreen");
    config_fontsize1 = get_config_int(variables, "fontsize1");
    config_fontsize2 = get_config_int(variables, "fontsize2");
    config_log_enabled = get_config_int(variables, "log_enabled");
    config_pasek_enabled = get_config_int(variables, "toolbar_show");
    config_chordsbase_on_startup = get_config_int(variables, "chordsbase_on_startup");
    //sprz¹tanie
    for(unsigned int i=0; i<variables->size(); i++){
        delete variables->at(i);
    }
    variables->clear();
}

vector<string>* Config::get_config_lines(string filename){
    vector<string>* lines = get_all_lines(filename);
    if(lines==NULL) return NULL;
    for(unsigned int i=0; i<lines->size(); i++){
        if(lines->at(i).length()==0){ //usuniêcie pustych elementów
            lines->erase(lines->begin()+i);
            i--;
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
                string name = lines->at(i).substr(0, i);
                string value = lines->at(i).substr(i+1);
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
        if(variables->at(i).name == name){
            return variables->at(i).value;
        }
    }
    Log::error("Nie znaleziono zmiennej w pliku konfiguracyjnym: "+name);
    return "";
}

int Config::get_config_int(vector<ConfigVariable*>* variables, string name){
    string s = get_config_string(variables, name);
    if(s.length()==0) return 0;
    return atoi(s.c_str());
}
