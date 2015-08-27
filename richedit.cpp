#include "app.h"
#include <richedit.h>

char* App::load_text(){
    /*
	str_size = GetWindowTextLength(hctrl[2]);
	char *str = new char[str_size+1];
	str[str_size]=0;
	GetWindowText(hctrl[2],str,str_size+1);
	get_selected_1(last_sel_start,last_sel_end);
	last_scroll=get_scroll();
	return str;
    */
    return NULL;
}

void App::save_text(char *str){
    /*
	SetWindowText(hctrl[2],str);
	format_text(str);
	//SetFocus(hctrl[2]);
	set_selected_1(last_sel_start,last_sel_end);
	delete[] str;
	set_scroll(last_scroll);
    */
}

char App::get_str_c(char *str, unsigned int str_size, int pos){
	if(pos<0||pos>=(int)str_size) return 0;
	return str[pos];
}

void App::del_char(char *&str, unsigned int &str_size, int &pos){
	if(pos<0||pos>=(int)str_size) return;
	char *new_str = new char [str_size];
	new_str[str_size-1]=0;
	for(int i=0; i<pos; i++){
		new_str[i] = str[i];
	}
	for(int i=pos+1; i<(int)str_size; i++){
		new_str[i-1] = str[i];
	}
	str_size--;
	pos--;
	//if(pos<0) pos=0;
	delete[] str;
	str = new_str;
}

void App::insert_char(char *&str, unsigned int &str_size, int pos, char c){
	if(pos<0||pos>(int)str_size) return;
	char *new_str = new char [str_size+1+1];
	new_str[str_size+1]=0;
	for(int i=0; i<pos; i++){
		new_str[i] = str[i];
	}
	new_str[pos]=c;
	for(int i=pos; i<(int)str_size; i++){
		new_str[i+1] = str[i];
	}
	str_size++;
	delete[] str;
	str = new_str;
}

bool App::get_selected(unsigned int &sel_start, unsigned int &sel_end, char *str){
    /*
	CHARRANGE ch;
	ch.cpMin = 0;
	ch.cpMax = 0;
	SendMessage(hctrl[2],EM_EXGETSEL,0,(LPARAM)&ch);
	sel_start = ch.cpMin;
	sel_end = ch.cpMax;
	//SendMessage(hctrl[2], EM_GETSEL, (WPARAM)&sel_start, (LPARAM)&sel_end);
	str_size = GetWindowTextLength(hctrl[2]);
	for(unsigned int i=0; i<sel_end&&i<str_size-1; i++){
		if(str[i]=='\r'&&str[i+1]=='\n'){
			sel_end++;
			if(i<sel_start) sel_start++;
		}
	}
	if(sel_start<0) sel_start=0;
	if(sel_end>str_size) sel_end=str_size;
	if(sel_start>=sel_end) return false;
	return true;
    */
    return true;
}

void App::set_selected(unsigned int sel_start, unsigned int sel_end, char *str){
    /*
	str_size = GetWindowTextLength(hctrl[2]);
	if(sel_start<0) sel_start=0;
	if(sel_end>str_size) sel_end=str_size;
	if(sel_end<sel_start) sel_end=sel_start;
	unsigned int temp_end = sel_end, temp_start = sel_start;
	if(temp_end>str_size-1) temp_end = str_size-1;
	for(unsigned int i=0; i<temp_end; i++){
		if(str[i]=='\r'&&str[i+1]=='\n'){
			sel_end--;
			if(i<temp_start) sel_start--;
		}
	}
	CHARRANGE ch;
	ch.cpMin=sel_start;
	ch.cpMax=sel_end;
	SendMessage(hctrl[2],EM_EXSETSEL,0,(LPARAM)&ch);
	//SendMessage(hctrl[2], EM_SETSEL, sel_start, sel_end);
    */
}

bool App::get_selected_1(unsigned int &sel_start, unsigned int &sel_end){
    /*
	CHARRANGE ch;
	ch.cpMin = 0;
	ch.cpMax = 0;
	SendMessage(hctrl[2],EM_EXGETSEL,0,(LPARAM)&ch);
	sel_start = ch.cpMin;
	sel_end = ch.cpMax;
	if(sel_start>=sel_end) return false;
    */
	return true;
}

void App::set_selected_1(unsigned int sel_start, unsigned int sel_end){
    /*
	CHARRANGE ch;
	ch.cpMin=sel_start;
	ch.cpMax=sel_end;
	SendMessage(hctrl[2],EM_EXSETSEL,0,(LPARAM)&ch);
    */
}

int App::get_scroll(){
    /*
	POINT mypoint;
	SendMessage(hctrl[2],EM_GETSCROLLPOS,0,(LPARAM)&mypoint);
	return mypoint.y;
    */
    return 0;
}

void App::set_scroll(int scroll_pos){
    /*
	POINT mypoint;
	mypoint.x = 0;
	mypoint.y = scroll_pos;
	SendMessage(hctrl[2],EM_SETSCROLLPOS,0,(LPARAM)&mypoint);
    */
}

void App::change_scroll(int c){
    /*
	int scroll = get_scroll() + c;
	if(scroll<0) scroll = 0;
	SCROLLINFO si;
	si.cbSize = sizeof(si);
	si.fMask = SIF_PAGE|SIF_RANGE|SIF_POS;
	GetScrollInfo(hctrl[2],SB_VERT,&si);
	int max_scroll = si.nMax - si.nPage;
	if(scroll>max_scroll) scroll = max_scroll;
	set_scroll(scroll);
    */
}

void App::select_all(){
    /*
	CHARRANGE ch;
	ch.cpMin=0;
	ch.cpMax=-1;
	SendMessage(hctrl[2],EM_EXSETSEL,0,(LPARAM)&ch);
	//str_size = GetWindowTextLength(hctrl[2]);
	//set_selected(0,str_size);
    */
}

void App::copy_text(){
    /*
	str_size = GetWindowTextLength(hctrl[2]);
	char *selected = new char [str_size];
	SendMessage(hctrl[2],EM_GETSELTEXT,0,(LPARAM)selected);
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
	echo("Skopiowano zaznaczony tekst");
    */
}
