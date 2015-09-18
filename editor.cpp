#include "app.h"
#include "strings.h"
#include <richedit.h>

string* App::load_text(){
	unsigned int str_size = get_editor_length();
	char *str = new char[str_size+1];
	str[str_size] = 0;
	GetWindowText(Controls::geti()->find("editor"), str, str_size+1);
    string* nowy = new string(str);
    delete[] str;
    get_selected(last_sel_start, last_sel_end, nowy);
    last_scroll = get_scroll();
	return nowy;
}

void App::save_text(string* str){
	Controls::geti()->set_text("editor", *str);
	format_text(str);
	set_selected(last_sel_start, last_sel_end, str);
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
	if(sel_start < 0) sel_start = 0;
	if(sel_end > str_size) sel_end = str_size;
    if(sel_start >= sel_end){
        sel_start = sel_end;
        return false;
    }
	return true;
}

bool App::get_selected_1(unsigned int &sel_start, unsigned int &sel_end){
	CHARRANGE ch;
	ch.cpMin = 0;
	ch.cpMax = 0;
	SendMessage(Controls::geti()->find("editor"), EM_EXGETSEL, 0, (LPARAM)&ch);
	sel_start = ch.cpMin;
	sel_end = ch.cpMax;
    if(sel_start >= sel_end){
        sel_start = sel_end;
        return false;
    }
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
	POINT scroll_pos;
	SendMessage(Controls::geti()->find("editor"), EM_GETSCROLLPOS, 0, (LPARAM)&scroll_pos);
	return scroll_pos.y;
}

void App::set_scroll(int scroll_pos){
	POINT mypoint;
	mypoint.x = 0;
	mypoint.y = scroll_pos;
	SendMessage(Controls::geti()->find("editor"), EM_SETSCROLLPOS, 0, (LPARAM)&mypoint);
}

bool App::change_scroll(int c){
    int scroll_pos = get_scroll();
    scroll_pos += c;
    if(scroll_pos<0) scroll_pos = 0;
    if(c > 0){
        SCROLLINFO si;
        si.cbSize = sizeof(si);
        si.fMask = SIF_PAGE|SIF_RANGE|SIF_POS;
        GetScrollInfo(Controls::geti()->find("editor"), SB_VERT, &si);
        //teoretycznie maksymalna wartoœæ (w praktyce siê pierdoli)
        int max_scroll = si.nMax - si.nPage + 1;
        //jeœli jest poza maksimum lub jeœli siê nie zgadza pozycja scrolla
        if(scroll_pos >= max_scroll || scroll_pos != si.nPos+c){
            //wyrównanie do maksymalnej pozycji scrolla
            SendMessage(Controls::geti()->find("editor"), WM_VSCROLL, SB_BOTTOM, 0);
            return false;
        }
    }
	set_scroll(scroll_pos);
    return true;
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
	Controls::geti()->set_focus(main_window);
	//t³o
	COLORREF background_color = RGB(Config::i()->color_background[0], Config::i()->color_background[1], Config::i()->color_background[2]);
	SendMessage(Controls::geti()->find("editor"), EM_SETBKGNDCOLOR, 0, (LPARAM)((COLORREF)background_color));
	//tekst
	CHARFORMAT cf_text;
	cf_text.cbSize = sizeof(cf_text);
    SendMessage(Controls::geti()->find("editor"), EM_GETCHARFORMAT, SCF_DEFAULT, (LPARAM)&cf_text);
	//kolor bia³y + brak pogrubienia
	cf_text.dwMask = CFM_COLOR | CFM_BOLD;
	cf_text.crTextColor = RGB(Config::i()->color_text[0], Config::i()->color_text[1], Config::i()->color_text[2]);
	cf_text.dwEffects = 0;
    SendMessage(Controls::geti()->find("editor"), EM_SETCHARFORMAT, SCF_ALL, (LPARAM)&cf_text);
	//style
    CHARFORMAT cf_nawias = CHARFORMAT(cf_text);
    CHARFORMAT cf_akord = CHARFORMAT(cf_text);
    cf_nawias.dwMask = CFM_COLOR;
    cf_nawias.crTextColor = RGB(Config::i()->color_bracket[0], Config::i()->color_bracket[1], Config::i()->color_bracket[2]);
    cf_nawias.dwEffects = 0;
    cf_akord.dwMask = CFM_COLOR | CFM_BOLD;
    cf_akord.crTextColor = RGB(Config::i()->color_chord[0], Config::i()->color_chord[1], Config::i()->color_chord[2]);
    cf_akord.dwEffects = CFE_BOLD;
    //kolorowanie nawiasów i akordów
    int nawias_start = -1;
	for(unsigned int i=0; i<str->length(); i++){
        if(str->at(i)==' ' || str->at(i)=='\n' || str->at(i)=='\r'){ //bia³e znaki
            continue;
        }else if(str->at(i)=='['){
			set_selected(i, i+1, str);
			SendMessage(Controls::geti()->find("editor"), EM_SETCHARFORMAT, SCF_SELECTION, (LPARAM)&cf_nawias);
            nawias_start = i+1;
		}else if(str->at(i)==']'){
            if(nawias_start != -1){ //by³ nawias otwieraj¹cy
                //kolorowanie akordu
                set_selected(nawias_start, i, str);
                SendMessage(Controls::geti()->find("editor"), EM_SETCHARFORMAT, SCF_SELECTION, (LPARAM)&cf_akord);
                nawias_start = -1; //reset nawiasu otwieraj¹cego
            }
            //kolorowanie nawiasu
			set_selected(i, i+1, str);
			SendMessage(Controls::geti()->find("editor"), EM_SETCHARFORMAT, SCF_SELECTION, (LPARAM)&cf_nawias);
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
	str[str_size] = 0;
	GetWindowText(Controls::geti()->find("editor"), str, str_size+1);
    string* nowy = new string(str);
    delete[] str;
	format_text(nowy);
    delete nowy;
}
