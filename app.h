#ifndef APP_H
#define APP_H

#include <windows.h>
#include <sstream>

using namespace std;

class App{
private:
	int ctrls_num;
	HWND hwnd;
public:
	App(int w, int h, string version, int ctrls = 0);
	~App();
	HWND *hctrl;
	HINSTANCE* hInst;
	int window_w, window_h;
	string version;
    HWND window2;
    void check_instance(char *classname);
	//io
	string *argv;
	int argc;
	void get_argv();
	bool is_arg(string par);
	void clear_file(string filename);
	void log(string l);
	void echo(string s);
	void echo(int e);
	int repeated_echo;
	string last_echo;
	void load_config();
	bool file_exists(string name);
	bool dir_exists(string name);
	void message(string m);
	void ss_clear(stringstream &sstream);
	stringstream ss;
	//config
	string songs_dir;
	int config_halfscreen;
    string config_fontface;
    int config_fontsize1;
    int config_fontsize2;
    int config_log_enabled;
    int config_pasek_enabled;
    int config_autoscroll_scale;
    int config_chordsbase_on_startup;
	//WM
	void wm_create(HWND *window);
	void wm_command(WPARAM wParam);
	void wm_dropfiles(char *filename);
	void wm_resize();
	void wm_screensave();
	void wm_keydown(WPARAM wParam);
	void wm_syskeydown(WPARAM wParam);
	void wm_timer();
    void wm_appcommand(WPARAM wParam, LPARAM lParam);
	//controls
	HWND create_button(string text, int x, int y, int w, int h, int button_nr);
	HWND create_edit(string text, int x, int y, int w, int h, int add_style);
	HWND create_static(string text, int x, int y, int w, int h, int add_style);
	void button_click(WPARAM wParam);
	//cmd
	string last_cmd, param;
	void exec_cmd();
	void exec_cmd(string str2);
	//pliki
	string opened_file;
	void new_file();
	void open_file(string filename);
	void save_file();
    void save_file_line(string filename, string line);
    string read_file_line(string filename);
	//edycja
	bool skanuj();
	void usun_akordy();
	void usun_wersje();
	void zamien();
	void znajdz();
	bool znajdz_w(char *str, unsigned int start, unsigned int end, char *wzor);
	void wstaw_tekst(int nrt);
	void zapisz_tekst(int nrt);
	string tekst_w[9];
	void dodaj_nawias();
	void dodaj_alternatywne();
	//formatowanie tekstu
	void change_font(HWND hwdp);
	void change_font_size(int change);
	int fontsize;
	string fontface;
	void format_text(char *str);
	void refresh_text();
	//richedit
	unsigned int str_size;
	char* load_text();
	void save_text(char *str);
	bool get_selected(unsigned int &sel_start, unsigned int &sel_end, char *str);
	void set_selected(unsigned int sel_start, unsigned int sel_end, char *str);
	bool get_selected_1(unsigned int &sel_start, unsigned int &sel_end);
	void set_selected_1(unsigned int sel_start, unsigned int sel_end);
	int get_scroll();
	void set_scroll(int scroll_pos);
	void change_scroll(int c);
	unsigned int last_sel_start, last_sel_end;
	int last_scroll;
	char get_str_c(char *str, unsigned int str_size, int pos);
	void del_char(char *&str, unsigned int &str_size, int &pos);
	void insert_char(char *&str, unsigned int &str_size, int pos, char c);
	void select_all();
	void copy_text();
	//transpozycja
	void transpose(int transponuj);
	int transposed;
	string transpose_string(string in, int t);
	//autoscroll
	bool autoscroll;
	int autoscroll_interval, autoscroll_wait, autoscroll_count;
	void autoscroll_exec();
	void autoscroll_on();
	void autoscroll_nowait(int change=0);
	void autoscroll_off();
	void autoscroll_switch();
	//subclassing
	WNDPROC windowProc, wndproc_new, *wndproc_old;
	LRESULT CALLBACK subclass_wndproc_new(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam);
	void subclass(int ctrl);
	void un_subclass(int ctrl);
	//program - inne funkcje
	void update_title();
	bool pasek_shown;
	void pasek_switch(int change=2);
	bool fullscreen_on;
	void fullscreen_set(bool full);
	void window_duplicate();
	void set_workdir();
    void quick_replace();
    string file_to_open;
    void chordsbase();
};

#endif // APP_H
