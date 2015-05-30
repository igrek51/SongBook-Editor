#include "app.h"

string get_config_string(string group, string name, string def, string filename){
	char *temp = new char [512];
	GetPrivateProfileString(group.c_str(),name.c_str(),def.c_str(),temp,512,filename.c_str());
	string result = temp;
	delete[] temp;
	return result;
}

void App::load_config(){
	log("Wczytywanie ustawieñ");
	if(!file_exists("conf.ini")) echo("! Brak pliku konfiguracyjnego");
	songs_dir = get_config_string("paths","songs_dir",".",".\\conf.ini");
	config_fontface = get_config_string("app_window","fontface","Calibri",".\\conf.ini");
	autoscroll_interval = GetPrivateProfileInt("autoscroll","autoscroll_interval",500,".\\conf.ini");
	autoscroll_wait = GetPrivateProfileInt("autoscroll","autoscroll_wait",35,".\\conf.ini");
	config_autoscroll_scale = GetPrivateProfileInt("autoscroll","autoscroll_scale",0,".\\conf.ini");
	config_halfscreen = GetPrivateProfileInt("app_window","halfscreen",0,".\\conf.ini");
    config_fontsize1 = GetPrivateProfileInt("app_window","fontsize1",18,".\\conf.ini");
    config_fontsize2 = GetPrivateProfileInt("app_window","fontsize2",18,".\\conf.ini");
    config_log_enabled = GetPrivateProfileInt("log","log_enabled",1,".\\conf.ini");
    config_pasek_enabled = GetPrivateProfileInt("app_window","toolbar_show",1,".\\conf.ini");
    config_chordsbase_on_startup = GetPrivateProfileInt("app_window","chordsbase_on_startup",0,".\\conf.ini");
}
