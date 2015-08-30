#ifndef CONFIG_H
#define CONFIG_H

#include <iostream>
#include <vector>

using namespace std;

class ConfigVariable {
public:
    ConfigVariable(string name, string value);
    string name;
    string value;
};

class Config {
private:
	static Config* instance;
    Config();
public:
    static Config* geti();
    static Config* i();
    void load_from_file();
    vector<string>* get_config_lines(string filename);
    vector<ConfigVariable*>* get_config_variables(string filename);
    string get_config_string(vector<ConfigVariable*>* variables, string name);
    int get_config_int(vector<ConfigVariable*>* variables, string name);
    bool get_config_bool(vector<ConfigVariable*>* variables, string name);
    //  Sta³e
    string config_filename;
    string log_filename;
    string program_name;
    string buttons_fontface;
    string find_edit_placeholder;
    string replace_edit_placeholder;
    int control_height;
    int toolbar_button_width;
    int static_filename_width;
    //  Zmienne
    int window_w;
    int window_h;
    int transposed;
    int autoscroll_count;
    int autoscroll_step;
	bool autoscroll;
    bool fullscreen_on;
    string opened_file; //atkualnie otwarty plik
    string file_to_open; //plik do otwarcia - polecenie z innej instancji
    string tekst_wstaw[9]; //schowki na akordy do wklejenia
    //  z pliku konfiguracyjnego
    string songs_dir;
    string editor_fontface;
    bool halfscreen;
    bool log_enabled;
    bool toolbar_show;
    bool autoscroll_scaling;
    bool chordsbase_on_startup;
    int buttons_fontsize;
    int editor_fontsize;
    int autoscroll_interval;
	int autoscroll_wait;
};

#endif
