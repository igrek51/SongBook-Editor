#include "app.h"
#include <richedit.h>

void App::change_font_size(int change){
    /*
	fontsize+=change;
	if(fontsize<1) fontsize=1;
	change_font(hctrl[2]);
	ss_clear(ss);
	ss<<"Rozmiar czcionki: "<<fontsize;
	if(config_autoscroll_scale==1){
		autoscroll_interval = autoscroll_interval*(fontsize-2*change)/(fontsize-change);
		stringstream ss2;
		ss2<<autoscroll_interval;
		SetWindowText(hctrl[19],ss2.str().c_str());
		ss<<", Interwa³ autoscrolla: "<<autoscroll_interval<<" ms";
	}
	echo(ss.str());
    */
}

void App::format_text(char *str){
    /*
	//log("format_text()...");
	unsigned int temp_sel_start, temp_sel_end, temp_scroll;
	get_selected_1(temp_sel_start,temp_sel_end);
	temp_scroll=get_scroll();
	str_size = GetWindowTextLength(hctrl[2]);
	SendMessage(hctrl[2],EM_HIDESELECTION,1,0);
	SetFocus(hctrl[0]);
	//t³o
	COLORREF color = RGB(0,0,0);
	SendMessage(hctrl[2],EM_SETBKGNDCOLOR,0,(LPARAM)((COLORREF)color));
	//kolor tekstu
	select_all();
	CHARFORMAT cf;
	cf.cbSize = sizeof(cf);
	SendMessage(hctrl[2],EM_GETCHARFORMAT,SCF_SELECTION,(LPARAM)&cf);
	//kolor bia³y
	cf.dwMask = CFM_COLOR;
	cf.crTextColor = RGB(255,255,255);
	cf.dwEffects = 0;
	SendMessage(hctrl[2],EM_SETCHARFORMAT,SCF_SELECTION,(LPARAM)&cf);
	SendMessage(hctrl[2],EM_SETCHARFORMAT,SCF_DEFAULT,(LPARAM)&cf);
	//brak pogrubienia
	cf.dwMask = CFM_BOLD;
	cf.dwEffects = 0;
	SendMessage(hctrl[2],EM_SETCHARFORMAT,SCF_SELECTION,(LPARAM)&cf);
	//formatowanie akordów
	int nawias=0;
	for(unsigned int i=0; i<str_size; i++){
		if(get_str_c(str,str_size,i)=='['){
			nawias=1;
			set_selected(i,i+1,str);
			SendMessage(hctrl[2],EM_GETCHARFORMAT,SCF_SELECTION,(LPARAM)&cf);
			//kolor szary
			cf.dwMask = CFM_COLOR;
			cf.crTextColor = RGB(180,180,180);
			cf.dwEffects = 0;
			SendMessage(hctrl[2],EM_SETCHARFORMAT,SCF_SELECTION,(LPARAM)&cf);
			continue;
		}
		if(get_str_c(str,str_size,i)==']'){
			nawias=0;
			set_selected(i,i+1,str);
			SendMessage(hctrl[2],EM_GETCHARFORMAT,SCF_SELECTION,(LPARAM)&cf);
			//kolor szary
			cf.dwMask = CFM_COLOR;
			cf.crTextColor = RGB(180,180,180);
			cf.dwEffects = 0;
			SendMessage(hctrl[2],EM_SETCHARFORMAT,SCF_SELECTION,(LPARAM)&cf);
			continue;
		}
		if(get_str_c(str,str_size,i)=='\r'||get_str_c(str,str_size,i)=='\n'||get_str_c(str,str_size,i)==' '){
			continue;
		}
		if(nawias==1){
			set_selected(i,i+1,str);
			SendMessage(hctrl[2],EM_GETCHARFORMAT,SCF_SELECTION,(LPARAM)&cf);
			//kolor czerwony
			cf.dwMask = CFM_COLOR;
			cf.crTextColor = RGB(255,0,0);
			cf.dwEffects = 0;
			SendMessage(hctrl[2],EM_SETCHARFORMAT,SCF_SELECTION,(LPARAM)&cf);
			//pogrubienie
			cf.dwMask = CFM_BOLD;
			cf.dwEffects = CFE_BOLD;
			SendMessage(hctrl[2],EM_SETCHARFORMAT,SCF_SELECTION,(LPARAM)&cf);
			continue;
		}
	}
	//przywrócenie zaznaczenia
	SetFocus(hctrl[2]);
	SendMessage(hctrl[2],EM_HIDESELECTION,0,0);
	set_selected_1(temp_sel_start,temp_sel_end);
	set_scroll(temp_scroll);
    */
}

void App::refresh_text(){
    /*
	str_size = GetWindowTextLength(hctrl[2]);
	char *str = new char[str_size+1];
	str[str_size]=0;
	GetWindowText(hctrl[2],str,str_size+1);
	format_text(str);
	delete[] str;
    */
}
