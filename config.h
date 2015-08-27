#ifndef CONFIG_H
#define CONFIG_H

#include <iostream>
#include <vector>

using namespace std;

class Config {
private:
	static Config* instance;
public:
    static Config* geti();
	Config();
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
    //  Zmienne
    int window_w;
    int window_h;
    int control_height;
    //  z pliku konfiguracyjnego
    string songs_dir;
    string editor_fontface;
    bool halfscreen;
    bool log_enabled;
    bool toolbar_show;
    bool autoscroll_scale;
    bool chordsbase_on_startup;
    int buttons_fontsize;
    int editor_fontsize;
    int autoscroll_interval;
	int autoscroll_wait;
};

class ConfigVariable {
public:
    ConfigVariable(string name, string value);
    string name;
    string value;
};

#endif
