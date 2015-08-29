#include "app.h"
#include "strings.h"
#include <richedit.h>

string* App::load_text(){
	unsigned int str_size = get_editor_length();
	char *str = new char[str_size+1];
	str[str_size]=0;
	GetWindowText(Controls::geti()->find("editor"), str, str_size+1);
	get_selected_1(last_sel_start, last_sel_end);
	last_scroll = get_scroll();
    string* nowy = new string(str);
    delete[] str;
	return nowy;
}

void App::save_text(string* str){
	Controls::geti()->set_text("editor", *str);
	format_text(str);
	set_selected_1(last_sel_start, last_sel_end);
	delete str;
	set_scroll(last_scroll);
}

unsigned int App::get_editor_length(){
    return GetWindowTextLength(Controls::geti()->find("editor"));
}

bool App::get_selected(unsigned int &sel_start, unsigned int &sel_end, string* str){
	CHARRANGE ch;
	ch.cpMin = 0;
	ch.cpMax = 0;
	SendMessage(Controls::geti()->find("editor"), EM_EXGETSEL, 0, (LPARAM)&ch);
	sel_start = ch.cpMin;
	sel_end = ch.cpMax;
	//SendMessage(hctrl[2], EM_GETSEL, (WPARAM)&sel_start, (LPARAM)&sel_end);
	unsigned int str_size = get_editor_length();
	for(unsigned int i=0; i<sel_end && i<str_size-1; i++){
		if(str->at(i)=='\r' && str->at(i+1)=='\n'){
			sel_end++;
			if(i<sel_start) sel_start++;
		}
	}
	if(sel_start<0) sel_start = 0;
	if(sel_end>str_size) sel_end = str_size;
	if(sel_start>=sel_end) return false;
	return true;
}

bool App::get_selected_1(unsigned int &sel_start, unsigned int &sel_end){
	CHARRANGE ch;
	ch.cpMin = 0;
	ch.cpMax = 0;
	SendMessage(Controls::geti()->find("editor"), EM_EXGETSEL, 0, (LPARAM)&ch);
	sel_start = ch.cpMin;
	sel_end = ch.cpMax;
	if(sel_start>=sel_end) return false;
	return true;
}

void App::set_selected(unsigned int sel_start, unsigned int sel_end, string* str){
	unsigned int str_size = get_editor_length();
	if(sel_start<0) sel_start=0;
	if(sel_end>str_size) sel_end=str_size;
	if(sel_end<sel_start) sel_end=sel_start;
	unsigned int temp_end = sel_end, temp_start = sel_start;
	if(temp_end>str_size-1) temp_end = str_size-1;
	for(unsigned int i=0; i<temp_end; i++){
		if(str->at(i)=='\r' && str->at(i+1)=='\n'){
			sel_end--;
			if(i<temp_start) sel_start--;
		}
	}
	CHARRANGE ch;
	ch.cpMin=sel_start;
	ch.cpMax=sel_end;
	SendMessage(Controls::geti()->find("editor"), EM_EXSETSEL, 0, (LPARAM)&ch);
	//SendMessage(hctrl[2], EM_SETSEL, sel_start, sel_end);
}

void App::set_selected_1(unsigned int sel_start, unsigned int sel_end){
	CHARRANGE ch;
	ch.cpMin = sel_start;
	ch.cpMax = sel_end;
	SendMessage(Controls::geti()->find("editor"), EM_EXSETSEL, 0, (LPARAM)&ch);
}

int App::get_scroll(){
	POINT mypoint;
	SendMessage(Controls::geti()->find("editor"), EM_GETSCROLLPOS, 0, (LPARAM)&mypoint);
	return mypoint.y;
}

void App::set_scroll(int scroll_pos){
	POINT mypoint;
	mypoint.x = 0;
	mypoint.y = scroll_pos;
	SendMessage(Controls::geti()->find("editor"), EM_SETSCROLLPOS, 0, (LPARAM)&mypoint);
}

void App::change_scroll(int c){
	int scroll = get_scroll() + c;
	if(scroll<0) scroll = 0;
	SCROLLINFO si;
	si.cbSize = sizeof(si);
	si.fMask = SIF_PAGE|SIF_RANGE|SIF_POS;
	GetScrollInfo(Controls::geti()->find("editor"), SB_VERT, &si);
	int max_scroll = si.nMax - si.nPage;
	if(scroll>max_scroll) scroll = max_scroll;
	set_scroll(scroll);
}

string App::get_selected_text(){
    char* selected = new char [get_editor_length()];
	SendMessage(Controls::geti()->find("editor"), EM_GETSELTEXT, 0, (LPARAM)selected);
    string result = selected;
    delete[] selected;
    return result;
}

void App::select_all(){
	CHARRANGE ch;
	ch.cpMin=0;
	ch.cpMax=-1;
	SendMessage(Controls::geti()->find("editor"), EM_EXSETSEL, 0, (LPARAM)&ch);
}

void App::copy_text(){
	string selected = trim_1crlf(get_selected_text());
    if(selected.length()==0){
		return;
	}
	const size_t len = selected.length() + 1;
	HGLOBAL hMem = GlobalAlloc(GMEM_MOVEABLE, len);
	memcpy(GlobalLock(hMem), selected.c_str(), len);
	GlobalUnlock(hMem);
	OpenClipboard(0);
	EmptyClipboard();
	SetClipboardData(CF_TEXT, hMem);
	CloseClipboard();
	IO::geti()->echo("Skopiowano zaznaczony tekst");
}


void App::format_text(string* str){
	IO::geti()->log("Formatowanie tekstu edytora...");
	unsigned int temp_sel_start, temp_sel_end, temp_scroll;
	get_selected_1(temp_sel_start, temp_sel_end);
	temp_scroll = get_scroll();
	SendMessage(Controls::geti()->find("editor"), EM_HIDESELECTION, 1, 0);
	Controls::geti()->set_focus("filename_edit");
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
	Controls::geti()->set_focus("editor");
	SendMessage(Controls::geti()->find("editor"), EM_HIDESELECTION, 0, 0);
	set_selected_1(temp_sel_start, temp_sel_end);
	set_scroll(temp_scroll);
}

void App::refresh_text(){
	unsigned int str_size = get_editor_length();
	char *str = new char[str_size+1];
	str[str_size]=0;
	GetWindowText(Controls::geti()->find("editor"), str, str_size+1);
    string* nowy = new string(str);
	format_text(nowy);
    delete[] str;
}
