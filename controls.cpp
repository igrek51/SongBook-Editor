#include "controls.h"

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

string Controls::get_button_name(int button_nr){
    if(button_nr>=1 && button_nr<=controls.size()){ //jeœli numer jest poprawny
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

void App::button_click(WPARAM wParam){
    //  TODO
    string name = get_button_name(wParam);
	if(name == "nowy"){ //nowy
		new_file();
	}
	if(wParam==12){ //wczytaj
		char *str2 = new char[512];
		GetWindowText(hctrl[0],str2,512);
		if(strlen(str2)==0){
			echo("Podaj nazwê pliku");
		}else{
			open_file(str2);
		}
		delete[] str2;
	}
	if(wParam==2){ //zapisz
		save_file();
	}
    if(wParam==3){ //analizuj
		int licznik=0;
		while(skanuj()) licznik++;
		if(licznik==0) echo("Brak zmian");
		else echo("Wprowadzono zmiany");
	}
	if(wParam==7){ //zamieñ
		zamien();
	}
	if(wParam==13){ //znajdŸ
		znajdz();
	}
    if(wParam==9){ //baza akordów
        chordsbase();
	}
	if(wParam==14){ //autoscroll
		autoscroll_switch();
	}
	if(wParam==16){ //schowanie paska
		if(fullscreen_on) fullscreen_set(false);
        else pasek_switch();
	}
}
