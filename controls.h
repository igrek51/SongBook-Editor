#ifndef CONTROLS_H
#define CONTROLS_H

#include <iostream>
#include <vector>
#define WINVER 0x0501 //WINXP
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

class Menu {
public:
    Menu();
    HMENU handle;
    void add_option(string display_text, string option_name);
    void add_separator();
    void add_menu(Menu* submenu, string display_text);
};

class Controls {
private:
	static Controls* instance;
    Controls();
public:
    static Controls* geti();
    static Controls* i();
    ~Controls();
    vector<Control*> controls;
    //  Wyszukiwanie kontrolek
    Control* find_control(string name);
    HWND find(string name);
    bool exists(string name);
    string get_button_name(int button_nr);
    //  Tworzenie nowych kontrolek
    void create_button(string text, int x, int y, int w, int h, string name = "");
    void create_button_multiline(string text, int x, int y, int w, int h, string name = "");
    void create_edit(string text, int x, int y, int w, int h, string name = "");
    void create_edit_center(string text, int x, int y, int w, int h, string name = "");
    void create_static(string text, int x, int y, int w, int h, string name = "");
    void create_static_center(string text, int x, int y, int w, int h, string name = "");
    void create_groupbox(string text, int x, int y, int w, int h);
    //  Zawartoœæ
    void set_text(string control_name, string text = "");
    void set_text(string control_name, int number);
    string get_text(string control_name);
    int get_int(string control_name);
    void select_all(string control_name);
    void set_focus(string control_name);
    void set_focus(HWND handle);
    //  Zmiana rozmiaru
    void resize(string control_name, int x, int y, int w=-1, int h=-1);
    //  Zmiana czcionki
    void set_font(HWND kontrolka, string fontface, int fontsize);
    void set_font(string name, string fontface, int fontsize);
    //  Menu
    int id_counter;
    vector<string> menu_names;
    vector<int> menu_ids;
    string get_menu_name(int menu_id);
};

#endif
