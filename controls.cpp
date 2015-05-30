#include "app.h"
#include <commctrl.h>

HWND App::create_button(string text, int x, int y, int w, int h, int button_nr){
	return CreateWindowEx(0,WC_BUTTON,text.c_str(),WS_CHILD|WS_VISIBLE,x,y,w,h,hwnd,(HMENU)button_nr,*hInst,0);
}

HWND App::create_edit(string text, int x, int y, int w, int h, int add_style){
	return CreateWindowEx(WS_EX_CLIENTEDGE,WC_EDIT,text.c_str(),WS_CHILD|WS_VISIBLE|ES_AUTOHSCROLL|add_style,x,y,w,h,hwnd,0,*hInst,0);
}

HWND App::create_static(string text, int x, int y, int w, int h, int add_style){
	return CreateWindowEx(0,WC_STATIC,text.c_str(),WS_CHILD|WS_VISIBLE|add_style,x,y,w,h,hwnd,0,*hInst,0);
}

void App::button_click(WPARAM wParam){
	if(wParam==1){ //nowy
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
