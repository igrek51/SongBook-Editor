#ifndef CONTROLS_H
#define CONTROLS_H

#include <iostream>
#include <vector>
#include "app.h"

using namespace std;

class Controls {
private:
	static Controls* instance;
public:
    static Controls* geti();
	Controls();
    ~Controls();
    vector<Control*> controls;
    HWND find(string name);
    string get_button_name(int button_nr);
    void set_text(string control_name, string text);
    void set_text(string control_name, int number);

    void create_button(string text, int x, int y, int w, int h, string name = "");
    void create_button_multiline(string text, int x, int y, int w, int h, string name = "");
    void create_edit(string text, int x, int y, int w, int h, string name = "");
    void create_edit_center(string text, int x, int y, int w, int h, string name = "");
    void create_static(string text, int x, int y, int w, int h, string name = "");
    void create_static_center(string text, int x, int y, int w, int h, string name = "");
    void create_groupbox(string text, int x, int y, int w, int h);
};

class Control {
public:
    Control(HWND handle, string name);
    ~Control();
    HWND handle;
    string name;
    WNDPROC wndproc_old;
}

#endif
