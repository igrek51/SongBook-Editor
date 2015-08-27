#ifndef CONTROLS_H
#define CONTROLS_H

#include <iostream>
#include <vector>
#include <windows.h>

using namespace std;

class Control {
public:
    Control(HWND handle, string name);
    ~Control();
    HWND handle;
    string name;
    WNDPROC wndproc_old;
};

class Controls {
private:
	static Controls* instance;
public:
    static Controls* geti();
	Controls();
    ~Controls();
    vector<Control*> controls;
    //  Wyszukiwanie kontrolek
    HWND find(string name);
    Control* find_control(string name);
    string get_button_name(int button_nr);
    //  Tworzenie nowych kontrolek
    void create_button(string text, int x, int y, int w, int h, string name = "");
    void create_button_multiline(string text, int x, int y, int w, int h, string name = "");
    void create_edit(string text, int x, int y, int w, int h, string name = "");
    void create_edit_center(string text, int x, int y, int w, int h, string name = "");
    void create_static(string text, int x, int y, int w, int h, string name = "");
    void create_static_center(string text, int x, int y, int w, int h, string name = "");
    void create_groupbox(string text, int x, int y, int w, int h);
    //  Zmiana zawartoœci
    void set_text(string control_name, string text);
    void set_text(string control_name, int number);
    //  Zmiana czcionki
    void set_font(HWND kontrolka, string fontface, int fontsize);
    void set_font(string name, string fontface, int fontsize);
};

#endif
