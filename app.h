#ifndef APP_H
#define APP_H

#include <iostream>
#include <sstream>
#define WINVER 0x0501 //WINXP
#include <windows.h>
#include "config.h"
#include "controls.h"
#include "files.h"
#include "io.h"
#include "music.h"
#include "undo.h"
#include "system.h"

using namespace std;

class App {
private:
    static App* instance;
    App();
public:
    static App* geti();
    static App* i();
	~App();
    HWND main_window;
    HWND instancja2;
	HINSTANCE* hInst;
	string version;
    void check_instance(string classname);
	//  EVENTS
	void event_init(HWND *window);
	void event_button(WPARAM wParam);
	void event_dropfiles(string filename);
	void event_resize();
	void event_screensave();
    void event_timer();
    void event_appcommand(WPARAM wParam, LPARAM lParam);
    bool event_syskeydown(WPARAM wParam);
	bool event_keydown(WPARAM wParam);
    bool event_close();
    //  subclassing
	WNDPROC windowProc, wndproc_new;
	LRESULT CALLBACK subclass_wndproc_new(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam);
    void subclass(Control* kontrolka);
    void subclass(string name);
    void un_subclass(Control* kontrolka);
    void un_subclass(string name);
	//  CMD
	string last_cmd;
	void exec_cmd();
	void exec_cmd(string cmd);
    void show_help();
	//  Edytor
    string* load_text();
    void save_text(string* str);
    unsigned int get_editor_length();
    unsigned int last_sel_start, last_sel_end;
	bool get_selected(unsigned int &sel_start, unsigned int &sel_end, string* str);
    bool get_selected_1(unsigned int &sel_start, unsigned int &sel_end);
	void set_selected(unsigned int sel_start, unsigned int sel_end, string* str);
	void set_selected_1(unsigned int sel_start, unsigned int sel_end);
    int last_scroll;
    int get_scroll();
    void set_scroll(int scroll_pos);
    bool change_scroll(int c);
    string get_selected_text();
    void select_all();
    void copy_text();
	void format_text(string* str);
	void refresh_text();
	//  autoscroll
	void autoscroll_exec();
	void autoscroll_on();
	void autoscroll_nowait(int change=0);
	void autoscroll_off();
	void autoscroll_switch();
    //  edycja
	bool skanuj_1();
	void usun_akordy();
	void usun_wersje();
	void zamien();
    bool znajdz_w(string* str, unsigned int start, unsigned int end, string wzor);
	void znajdz();
	void wstaw_tekst(int nrt);
	void zapisz_tekst(int nrt);
	void dodaj_nawias();
	void dodaj_alternatywne();
    void save_pattern();
    void insert_pattern();
    vector<string> pattern;
    //  UNDO
    Undo* undo;
	//  program - inne funkcje
    void update_title();
    void controls_fonts_set();
    void change_font_size(int change);
    void toolbar_switch(int change = 2);
    void cmd_switch(int change = 2);
    void fullscreen_set(bool full);
    void fullscreen_toggle();
    void chordsbase_start();
    void quick_replace();
    void new_file();
    void open_chords_file(string filename);
	bool save_chords_file();
    void analyze();
    void transpose(int transponuj);
};

#endif // APP_H
