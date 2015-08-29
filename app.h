#ifndef APP_H
#define APP_H

#include <iostream>
#include <sstream>
#include <windows.h>
#include "config.h"
#include "controls.h"
#include "files.h"
#include "io.h"
#include "system.h"
#include "music.h"

using namespace std;

class App {
private:
    static App* instance;
    App();
public:
    static App* geti();
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
    void event_syskeydown(WPARAM wParam);
	void event_keydown(WPARAM wParam);
	//  CMD
	string last_cmd;
	void exec_cmd();
	void exec_cmd(string cmd);
	//  edycja
    void analyze();
	bool skanuj();
	void usun_akordy();
	void usun_wersje();
	void zamien();
	void znajdz();
	bool znajdz_w(string* str, unsigned int start, unsigned int end, string wzor);
	void wstaw_tekst(int nrt);
	void zapisz_tekst(int nrt);
	void dodaj_nawias();
	void dodaj_alternatywne();
	//  Editor
    void change_font_size(int change);
	void format_text(string* str);
	void refresh_text();
    string* load_text();
    void save_text(string* str);
	bool get_selected(unsigned int &sel_start, unsigned int &sel_end, string* str);
	void set_selected(unsigned int sel_start, unsigned int sel_end, string* str);
	bool get_selected_1(unsigned int &sel_start, unsigned int &sel_end);
	void set_selected_1(unsigned int sel_start, unsigned int sel_end);
	int get_scroll();
	void set_scroll(int scroll_pos);
	void change_scroll(int c);
	unsigned int last_sel_start, last_sel_end;
	int last_scroll;
	char string_char(string* str, int pos);
    void string_char_set(string* str, int pos, char c);
	void string_delete(string*& str, int &pos);
	void string_insert(string*& str, int pos, char c);
	void select_all();
	void copy_text();
	//  transpozycja
	void transpose(int transponuj);
	//  autoscroll
	void autoscroll_exec();
	void autoscroll_on();
	void autoscroll_nowait(int change=0);
	void autoscroll_off();
	void autoscroll_switch();
	//  subclassing
	WNDPROC windowProc, wndproc_new;
	LRESULT CALLBACK subclass_wndproc_new(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam);
    void subclass(Control* kontrolka);
    void subclass(string name);
    void un_subclass(Control* kontrolka);
    void un_subclass(string name);
	//  program - inne funkcje
    void new_file();
	void open_file(string filename);
	void save_file();
	void update_title();
    void controls_fonts_set();
	void toolbar_switch(int change=2);
	void fullscreen_set(bool full);
    void quick_replace();
    void chordsbase();
};

#endif // APP_H
