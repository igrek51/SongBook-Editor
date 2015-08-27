#include "controls.h"
#include <commctrl.h>
#include <richedit.h>

Controls* Controls::instance = NULL;

Controls* Controls::geti(){
    if(instance == NULL){
        instance = new Controls();
    }
    return instance;
}

Controls::~Controls(){
    for(unsigned int i=0; i<controls.size(); i++){
        delete controls.at(i);
    }
    controls.clear();
}


Control::Control(HWND handle, string name){
    this->handle = handle;
    this->name = name;
    wndproc_old = NULL;
}

Control::~Control(){
    //  TODO
    un_subclass(i);
    if(hctrl[i]!=NULL) DestroyWindow(hctrl[i]);

}


HWND Controls::find(string name){
    if(name.length()==0) return NULL;
    for(unsigned int i=0; i<controls.size(); i++){
        if(controls.at(i)->name == name){
            return controls.at(i)->handle;
        }
    }
    IO::geti()->error("Nie odnaleziono kontrolki o nazwie: "+name);
    return NULL;
}

Control* Controls::find_control(string name){
    if(name.length()==0) return NULL;
    for(unsigned int i=0; i<controls.size(); i++){
        if(controls.at(i)->name == name){
            return controls.at(i);
        }
    }
    IO::geti()->error("Nie odnaleziono kontrolki o nazwie: "+name);
    return NULL;
}

string Controls::get_button_name(int button_nr){
    //je�li numer jest poprawny
    if(button_nr>=1 && button_nr<=controls.size()){
        return controls.at(button_nr-1)->name;
    }
    IO::geti()->error("Nie odnaleziono kontrolki o numerze: "+button_nr);
    return "";
}

void Controls::create_button(string text, int x, int y, int w, int h, string name){
    int button_nr = controls.size() + 1;
    HWND handle = CreateWindowEx(0, WC_BUTTON, text.c_str(), WS_CHILD|WS_VISIBLE, x, y, w, h, App::geti()->main_window, (HMENU)button_nr, *App::geti()->hInst, 0);
    controls.push_back(new Control(handle, name));
}

void Controls::create_button_multiline(string text, int x, int y, int w, int h, string name){
    int button_nr = controls.size() + 1;
    HWND handle = CreateWindowEx(0, WC_BUTTON, text.c_str(), WS_CHILD|WS_VISIBLE|BS_MULTILINE, x, y, w, h, App::geti()->main_window, (HMENU)button_nr, *App::geti()->hInst, 0);
    controls.push_back(new Control(handle, name));
}

void Controls::create_edit(string text, int x, int y, int w, int h, string name){
	HWND handle = CreateWindowEx(WS_EX_CLIENTEDGE, WC_EDIT, text.c_str(), WS_CHILD|WS_VISIBLE|ES_AUTOHSCROLL, x, y, w, h, App::geti()->main_window, 0, *App::geti()->hInst, 0);
    controls.push_back(new Control(handle, name));
}

void Controls::create_edit_center(string text, int x, int y, int w, int h, string name){
	HWND handle = CreateWindowEx(WS_EX_CLIENTEDGE, WC_EDIT, text.c_str(), WS_CHILD|WS_VISIBLE|ES_AUTOHSCROLL|ES_CENTER, x, y, w, h, App::geti()->main_window, 0, *App::geti()->hInst, 0);
    controls.push_back(new Control(handle, name));
}

void Controls::create_static(string text, int x, int y, int w, int h, string name){
	HWND handle = CreateWindowEx(0, WC_STATIC, text.c_str(), WS_CHILD|WS_VISIBLE, x, y, w, h, App::geti()->main_window, 0, *App::geti()->hInst, 0);
    controls.push_back(new Control(handle, name));
}

void Controls::create_static_center(string text, int x, int y, int w, int h, string name){
	HWND handle = CreateWindowEx(0, WC_STATIC, text.c_str(), WS_CHILD|WS_VISIBLE|SS_CENTER|SS_CENTERIMAGE, x, y, w, h, App::geti()->main_window, 0, *App::geti()->hInst, 0);
    controls.push_back(new Control(handle, name));
}

void Controls::create_groupbox(string text, int x, int y, int w, int h){
    HWND handle = CreateWindowEx(0, WC_BUTTON, text.c_str(), WS_CHILD|WS_VISIBLE|BS_GROUPBOX, x, y, w, h, App::geti()->main_window, 0, *App::geti()->hInst, 0);
    controls.push_back(new Control(handle, name));
}


void Controls::set_text(string control_name, string text){
    SetWindowText(find(control_name), text.c_str());
	//UpdateWindow(hwnd);
}

void Controls::set_text(string control_name, int number){
    stringstream ss;
    ss<<number;
    set_text(control_name, ss.str());
}

void Controls::set_font(HWND kontrolka, string fontface, int fontsize){
    if(kontrolka==NULL) return;
    HFONT hFont = CreateFont(fontsize, 0, 0, 0, FW_NORMAL, 0, 0, 0, ANSI_CHARSET, 0, 0, 0, 0, fontface.c_str());
    //HFONT hFont_old = SendMessage(kontrolka, WM_GETFONT, 0, 0);
	SendMessage(kontrolka, WM_SETFONT, (WPARAM)hFont, true);
    //usuni�cie starej czcionki
    //  TODO
	//if(hwdp==hctrl[2]) refresh_text();
}

void Controls::set_font(string name, string fontface, int fontsize){
    set_font(find(name), fontface, fontsize)
}
