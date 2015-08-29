#include "app.h"
#include <richedit.h>

string* App::load_text(){
	str_size = GetWindowTextLength(Controls::geti()->find("editor"));
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
	SetWindowText(Controls::geti()->find("editor"), str->c_str());
	format_text(str);
	//SetFocus(hctrl[2]);
	set_selected_1(last_sel_start, last_sel_end);
	delete str;
	set_scroll(last_scroll);
}

char App::string_char(string* str, int pos){
	if(pos<0 || pos>=(int)str->length()) return 0;
	return str->at(pos);
}

void App::string_delete(string*& str, int &pos){
	if(pos<0 || pos>=(int)str->length()) return;
    str->erase(pos, 1);
	pos--;
	//if(pos<0) pos=0;
}

void App::string_insert(string*& str, int pos, char c){
	if(pos<0 || pos>(int)str->length()) return;
    str->insert(str->begin()+pos, c);
}

bool App::get_selected(unsigned int &sel_start, unsigned int &sel_end, string* str){
	CHARRANGE ch;
	ch.cpMin = 0;
	ch.cpMax = 0;
	SendMessage(Controls::geti()->find("editor"), EM_EXGETSEL, 0, (LPARAM)&ch);
	sel_start = ch.cpMin;
	sel_end = ch.cpMax;
	//SendMessage(hctrl[2], EM_GETSEL, (WPARAM)&sel_start, (LPARAM)&sel_end);
	str_size = GetWindowTextLength(Controls::geti()->find("editor"));
	for(unsigned int i=0; i<sel_end&&i<str_size-1; i++){
		if(str->at(i)=='\r' && str->at(i+1)=='\n'){
			sel_end++;
			if(i<sel_start) sel_start++;
		}
	}
	if(sel_start<0) sel_start=0;
	if(sel_end>str_size) sel_end=str_size;
	if(sel_start>=sel_end) return false;
	return true;
}

void App::set_selected(unsigned int sel_start, unsigned int sel_end, string* str){
	str_size = GetWindowTextLength(Controls::geti()->find("editor"));
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

void App::set_selected_1(unsigned int sel_start, unsigned int sel_end){
	CHARRANGE ch;
	ch.cpMin=sel_start;
	ch.cpMax=sel_end;
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

void App::select_all(){
	CHARRANGE ch;
	ch.cpMin=0;
	ch.cpMax=-1;
	SendMessage(Controls::geti()->find("editor"), EM_EXSETSEL, 0, (LPARAM)&ch);
	//str_size = GetWindowTextLength(hctrl[2]);
	//set_selected(0,str_size);
}

void App::copy_text(){
	str_size = GetWindowTextLength(Controls::geti()->find("editor"));
	char *selected = new char [str_size];
	SendMessage(Controls::geti()->find("editor"), EM_GETSELTEXT, 0, (LPARAM)selected);
	if(strlen(selected)==0){
		delete[] selected;
		return;
	}
	const size_t len = strlen(selected) + 1;
	HGLOBAL hMem =  GlobalAlloc(GMEM_MOVEABLE, len);
	memcpy(GlobalLock(hMem), selected, len);
	GlobalUnlock(hMem);
	OpenClipboard(0);
	EmptyClipboard();
	SetClipboardData(CF_TEXT, hMem);
	CloseClipboard();
	delete[] selected;
	IO::geti()->echo("Skopiowano zaznaczony tekst");
}
