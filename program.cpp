#include "app.h"
#include <richedit.h>

void App::update_title(){
	ss_clear(ss);
	ss<<"Igrek SongBook Editor v"<<version<<": ";
	if(opened_file.length()==0){
		ss<<"[Nowy plik]";
	}else{
		ss<<opened_file;
	}
	SetWindowText(hwnd,ss.str().c_str());
}

void App::pasek_switch(int change){
	if(change==1){
			pasek_shown = true;
	}else if(change==0){
			pasek_shown = false;
	}else if(change==2){
			pasek_shown=!pasek_shown;
	}
	wm_resize();
	if(pasek_shown){
		SetWindowText(hctrl[22],"^");
		for(int i=0; i<ctrls_num; i++){
			if(i==1) continue;
			if(i==2) continue;
			if(i==22) continue;
			ShowWindow(hctrl[i],SW_SHOW);
		}
	}else{
		SetWindowText(hctrl[22],"v");
		for(int i=0; i<ctrls_num; i++){
			if(i==1) continue;
			if(i==2) continue;
			if(i==22) continue;
			ShowWindow(hctrl[i],SW_HIDE);
		}
	}
}

void App::fullscreen_set(bool full){
	fullscreen_on = full;
	if(fullscreen_on){
		ShowWindow(hwnd,SW_MAXIMIZE);
	}else{
		ShowWindow(hwnd,SW_RESTORE);
	}
}

void App::window_duplicate(){
	ss_clear(ss);
	if(opened_file.length()>0) ss<<"--fo \""<<opened_file<<"\"";
	char *app_path = new char [512];
	GetModuleFileName(NULL, app_path, 512);
	ShellExecute(0,"open",app_path,ss.str().c_str(),"",SW_SHOW);
	delete[] app_path;
}

void App::quick_replace(){
	char *str = load_text();
	char *selected = new char [str_size];
	SendMessage(hctrl[2],EM_GETSELTEXT,0,(LPARAM)selected);
	char c;
	while(strlen(selected)>0){
		c = selected[0];
		if(c==' '||c=='\n'||c=='\r'){
			selected++;
			continue;
		}
		c = selected[strlen(selected)-1];
		if(c==' '||c=='\n'||c=='\r'){
			selected[strlen(selected)-1]=0;
			continue;
		}
		break;
	}
	if(strlen(selected)==0){
			echo("! B³¹d: Pusty tekst do zamiany.");
			delete[] selected;
			delete[] str;
			return;
	}
	set_selected(last_sel_end,last_sel_end,str);
	SetWindowText(hctrl[9],selected);
	SetWindowText(hctrl[10],selected);
	SetFocus(hctrl[10]);
	delete[] selected;
	delete[] str;
}

void App::chordsbase(){
    if(songs_dir.length()>0) ShellExecute(0,"open",songs_dir.c_str(),"","",SW_SHOW);
}
