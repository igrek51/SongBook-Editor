#include "app.h"
#include <richedit.h>

void App::format_text(string* str){
	IO::geti()->log("Formatowanie tekstu edytora...");
	unsigned int temp_sel_start, temp_sel_end, temp_scroll;
	get_selected_1(temp_sel_start, temp_sel_end);
	temp_scroll = get_scroll();
	SendMessage(Controls::geti()->find("editor"), EM_HIDESELECTION, 1, 0);
	SetFocus(Controls::geti()->find("cmd"));
	//t³o
	COLORREF color = RGB(0,0,0);
	SendMessage(Controls::geti()->find("editor"), EM_SETBKGNDCOLOR, 0, (LPARAM)((COLORREF)color));
	//kolor tekstu
	select_all();
	CHARFORMAT cf;
	cf.cbSize = sizeof(cf);
	SendMessage(Controls::geti()->find("editor"), EM_GETCHARFORMAT, SCF_SELECTION, (LPARAM)&cf);
	//kolor bia³y
	cf.dwMask = CFM_COLOR;
	cf.crTextColor = RGB(255,255,255);
	cf.dwEffects = 0;
	SendMessage(Controls::geti()->find("editor"), EM_SETCHARFORMAT, SCF_SELECTION, (LPARAM)&cf);
	SendMessage(Controls::geti()->find("editor"), EM_SETCHARFORMAT, SCF_DEFAULT, (LPARAM)&cf);
	//brak pogrubienia
	cf.dwMask = CFM_BOLD;
	cf.dwEffects = 0;
	SendMessage(Controls::geti()->find("editor"), EM_SETCHARFORMAT, SCF_SELECTION, (LPARAM)&cf);
	//formatowanie akordów
	int nawias = 0;
	for(unsigned int i=0; i<str->length(); i++){
		if(string_char(str, i)=='['){
			nawias = 1;
			set_selected(i, i+1, str);
			SendMessage(Controls::geti()->find("editor"), EM_GETCHARFORMAT, SCF_SELECTION, (LPARAM)&cf);
			//kolor szary
			cf.dwMask = CFM_COLOR;
			cf.crTextColor = RGB(180,180,180);
			cf.dwEffects = 0;
			SendMessage(Controls::geti()->find("editor"), EM_SETCHARFORMAT, SCF_SELECTION, (LPARAM)&cf);
			continue;
		}
		if(string_char(str, i)==']'){
			nawias = 0;
			set_selected(i, i+1, str);
			SendMessage(Controls::geti()->find("editor"), EM_GETCHARFORMAT, SCF_SELECTION, (LPARAM)&cf);
			//kolor szary
			cf.dwMask = CFM_COLOR;
			cf.crTextColor = RGB(180,180,180);
			cf.dwEffects = 0;
			SendMessage(Controls::geti()->find("editor"), EM_SETCHARFORMAT, SCF_SELECTION, (LPARAM)&cf);
			continue;
		}
		if(string_char(str, i)=='\r' || string_char(str, i)=='\n' || string_char(str, i)==' '){
			continue;
		}
		if(nawias == 1){
			set_selected(i, i+1, str);
			SendMessage(Controls::geti()->find("editor"), EM_GETCHARFORMAT, SCF_SELECTION, (LPARAM)&cf);
			//kolor czerwony
			cf.dwMask = CFM_COLOR;
			cf.crTextColor = RGB(255,0,0);
			cf.dwEffects = 0;
			SendMessage(Controls::geti()->find("editor"), EM_SETCHARFORMAT, SCF_SELECTION, (LPARAM)&cf);
			//pogrubienie
			cf.dwMask = CFM_BOLD;
			cf.dwEffects = CFE_BOLD;
			SendMessage(Controls::geti()->find("editor"), EM_SETCHARFORMAT, SCF_SELECTION, (LPARAM)&cf);
			continue;
		}
	}
	//przywrócenie zaznaczenia
	SetFocus(Controls::geti()->find("editor"));
	SendMessage(Controls::geti()->find("editor"), EM_HIDESELECTION, 0, 0);
	set_selected_1(temp_sel_start, temp_sel_end);
	set_scroll(temp_scroll);
}

void App::refresh_text(){
	unsigned int str_size = GetWindowTextLength(Controls::geti()->find("editor"));
	char *str = new char[str_size+1];
	str[str_size]=0;
	GetWindowText(Controls::geti()->find("editor"), str, str_size+1);
    string* nowy = new string(str);
	format_text(nowy);
    delete[] str;
}
