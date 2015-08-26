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
    //  Zmienne
    string songs_dir;
    string fontface;
    bool halfscreen;
    bool log_enabled;
    bool toolbar_show;
    bool autoscroll_scale;
    bool chordsbase_on_startup;
    int fontsize1;
    int fontsize2;
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
