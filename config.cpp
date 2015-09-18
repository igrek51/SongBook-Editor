#include "config.h"
#include "files.h"
#include "io.h"
#include "strings.h"
#include <cstdlib>

Config* Config::instance = NULL;

Config* Config::geti(){
    if(instance == NULL){
        instance = new Config();
    }
    return instance;
}

Config* Config::i(){
    return geti();
}

Config::Config(){
    //Sta³e
    config_filename = "conf.ini";
    log_filename = "log.txt";
    buttons_fontface = "MS Shell Dlg 2";
    program_name = "Igrek SongBook Editor";
    find_edit_placeholder = "Szukaj...";
    replace_edit_placeholder = "Zamieñ...";
    output_control = "cmd_output1";
    explorer_classname = "CabinetWClass";
    control_height = 22;
    static_filename_width = 40;
    cmd_outputs_num = 5;
    cmd_outputs_space = 5;
    chordsbase_wait = 50;
    chordsbase_max_waits = 15;
    //Zmienne
    window_w = 600;
    window_h = 680;
    transposed = 0;
    autoscroll_count = 0;
    autoscroll_step = 2;
	autoscroll = false;
    fullscreen_on = false;
    cmd_show = false;
	opened_file = "";
    file_to_open = "";
	for(int i=0; i<9; i++) tekst_wstaw[i]="";
    cmd_outputs = new string[cmd_outputs_num];
    //Kolory
    color_background[0] = 0;
    color_background[1] = 0;
    color_background[2] = 0;
    color_text[0] = 255;
    color_text[1] = 255;
    color_text[2] = 255;
    color_bracket[0] = 180;
    color_bracket[1] = 180;
    color_bracket[2] = 180;
    color_chord[0] = 255;
    color_chord[1] = 0;
    color_chord[2] = 0;
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
        IO::geti()->message_box("B³¹d", "Brak pliku konfiguracyjnego - ³adowanie wartoœci domyœlnych");
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
    delete variables;
    IO::geti()->log("Wczytano ustawienia z pliku: "+Config::geti()->config_filename);
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
                string name = trim_spaces(lines->at(i).substr(0, j));
                string value = trim_quotes(trim_spaces(lines->at(i).substr(j+1)));
                variables->push_back(new ConfigVariable(name, value));
                break;
            }
        }
    }
    lines->clear();
    return variables;
}

string Config::get_config_string(vector<ConfigVariable*>* variables, string name, string domyslny){
    if(variables==NULL) return domyslny;
    for(unsigned int i=0; i<variables->size(); i++){
        if(variables->at(i)->name == name){
            return variables->at(i)->value;
        }
    }
    IO::geti()->error("Nie znaleziono zmiennej w pliku konfiguracyjnym: "+name);
    return domyslny;
}

int Config::get_config_int(vector<ConfigVariable*>* variables, string name, int domyslny){
    string s = get_config_string(variables, name);
    if(s.length()==0) return domyslny;
    return atoi(s.c_str());
}

bool Config::get_config_bool(vector<ConfigVariable*>* variables, string name, bool domyslny){
    string s = get_config_string(variables, name);
    if(s.length()==0) return domyslny;
    if(s=="true") return true;
    if(s=="1") return true;
    return false;
}
