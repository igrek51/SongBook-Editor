#ifndef CONFIG_H
#define CONFIG_H

#include <iostream>

using namespace std;

class Config{
private:
	static Config* instance;
public:
    static Config* geti();
	Config();

    string songs_dir;
	int config_halfscreen;
    string config_fontface;
    int config_fontsize1;
    int config_fontsize2;
    int config_log_enabled;
    int config_pasek_enabled;
    int config_autoscroll_scale;
    int config_chordsbase_on_startup;
    int autoscroll_interval;
	int autoscroll_wait;
};

#endif
