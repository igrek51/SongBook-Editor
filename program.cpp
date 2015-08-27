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
        Config::geti()->toolbar_show = true;
    }else if(change==0){
        Config::geti()->toolbar_show = false;
    }else if(change==2){
        Config::geti()->toolbar_show = !Config::geti()->toolbar_show;
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


void App::new_file(){
	SetWindowText(hctrl[2],"");
	refresh_text();
	opened_file="";
	update_title();
	echo("Nowy plik");
}

void App::open_file(string filename){
	if(!file_exists(filename)){
		ss_clear(ss);
		ss<<"! B³¹d: plik \""<<filename<<"\" nie istnieje";
		echo(ss.str());
		return;
	}
	fstream plik;
	plik.open(filename.c_str(),fstream::in|fstream::binary);
	if(!plik.good()){
		echo("! B³¹d otwarcia pliku");
		plik.close();
		return;
	}
	plik.seekg(0,plik.end);
	unsigned int fsize = plik.tellg();
	char *file_content = new char [fsize+1];
	file_content[fsize]=0;
	plik.seekg(0,plik.beg);
	plik.read(file_content,fsize);
	plik.close();
	SetWindowText(hctrl[2],file_content);
	set_selected(0,0,file_content);
	SendMessage(hctrl[2], EM_SCROLLCARET, 0, 0);
	opened_file = filename;
	ss_clear(ss);
	ss<<"Odczytano plik: "<<filename;
	echo(ss.str());
	SetWindowText(hctrl[0],filename.c_str());
	refresh_text();
	update_title();
	transposed=0;
	autoscroll_off();
}

void App::save_file(){
	char *new_filename = new char[256];
	GetWindowText(hctrl[0],new_filename,256);
	if(strlen(new_filename)==0){
		echo("Podaj nazwê pliku");
		delete[] new_filename;
		return;
	}
	opened_file = new_filename;
	delete[] new_filename;
	fstream plik;
	plik.open(opened_file.c_str(),fstream::out|fstream::binary);
	if(!plik.good()){
		echo("! B³¹d œcie¿ki pliku");
		plik.close();
		return;
	}
	char *str2 = load_text();
	plik.write(str2,str_size);
	plik.close();
	delete[] str2;
	update_title();
	ss_clear(ss);
	ss<<"Zapisano plik";
	if(transposed%12!=0){
		ss<<" (Zapis w nowej tonacji: ";
		if(transposed>0) ss<<"+";
		ss<<transposed;
		ss<<" !)";
	}
	echo(ss.str());
}
