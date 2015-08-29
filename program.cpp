#include "app.h"
#include <richedit.h>
#include <fstream>

void App::update_title(){
	stringstream ss;
	ss<<Config::geti()->program_name<<" v"<<version<<": ";
	if(Config::geti()->opened_file.length()==0){
		ss<<"[Nowy plik]";
	}else{
		ss<<Config::geti()->opened_file;
	}
	SetWindowText(main_window, ss.str().c_str());
}

void App::controls_fonts_set(){
    for(unsigned int i=0; i<Controls::geti()->controls.size(); i++){
        string fontface = Config::geti()->buttons_fontface;
        int fontsize = Config::geti()->buttons_fontsize;
        if(Controls::geti()->controls.at(i)->name == "editor"){
            fontface = Config::geti()->editor_fontface;
            fontsize = Config::geti()->editor_fontsize;
        }
        Controls::geti()->set_font(Controls::geti()->controls.at(i)->handle, fontface, fontsize);
    }
}

void App::toolbar_switch(int change){
    if(change==1){
        Config::geti()->toolbar_show = true;
    }else if(change==0){
        Config::geti()->toolbar_show = false;
    }else if(change==2){
        Config::geti()->toolbar_show = !Config::geti()->toolbar_show;
    }
    event_resize();
    if(Config::geti()->toolbar_show){
        IO::geti()->log("Ods³anianie paska narzêdzi...");
        Controls::geti()->set_text("toolbar_toggle", "^");
        for(unsigned int i=0; i<Controls::geti()->controls.size(); i++){
            Control* kontrolka = Controls::geti()->controls.at(i);
            if(kontrolka->name=="statusbar") continue;
            if(kontrolka->name=="editor") continue;
            if(kontrolka->name=="toolbar_toggle") continue;
            ShowWindow(kontrolka->handle, SW_SHOW);
        }
    }else{
        IO::geti()->log("Ukrywanie paska narzêdzi...");
        Controls::geti()->set_text("toolbar_toggle", "v");
        for(unsigned int i=0; i<Controls::geti()->controls.size(); i++){
            Control* kontrolka = Controls::geti()->controls.at(i);
            if(kontrolka->name=="statusbar") continue;
            if(kontrolka->name=="editor") continue;
            if(kontrolka->name=="toolbar_toggle") continue;
            ShowWindow(kontrolka->handle,SW_HIDE);
        }
    }
}

void App::fullscreen_set(bool full){
	Config::geti()->fullscreen_on = full;
	if(Config::geti()->fullscreen_on){
		ShowWindow(main_window, SW_MAXIMIZE);
        IO::geti()->log("Fullscreen w³¹czony.");
	}else{
		ShowWindow(main_window, SW_RESTORE);
        IO::geti()->log("Fullscreen wy³¹czony.");
	}
}

void App::quick_replace(){
	string* str = load_text();
	char *selected = new char [str->length()];
	SendMessage(Controls::geti()->find("editor"), EM_GETSELTEXT, 0, (LPARAM)selected);
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
        IO::geti()->echo("! B³¹d: Pusty tekst do zamiany.");
        delete[] selected;
        delete str;
        return;
    }
    set_selected(last_sel_end, last_sel_end, str);
    Controls::geti()->set_text("find_edit", selected);
    Controls::geti()->set_text("replace_edit", selected);
    SetFocus(Controls::geti()->find("replace_edit"));
    delete[] selected;
	delete str;
}

void App::chordsbase(){
    IO::geti()->log("Otwieranie bazy akordów...");
    if(Config::geti()->songs_dir.length()>0){
        ShellExecute(0, "open", Config::geti()->songs_dir.c_str(), "", "", SW_SHOW);
    }
}


void App::new_file(){
	SetWindowText(Controls::geti()->find("editor"), "");
	refresh_text();
	Config::geti()->opened_file = "";
	update_title();
	IO::geti()->echo("Nowy plik");
}

void App::open_file(string filename){
	if(!file_exists(filename)){
		IO::geti()->error("plik \""+filename+"\" nie istnieje");
		return;
	}
	fstream plik;
	plik.open(filename.c_str(),fstream::in|fstream::binary);
	if(!plik.good()){
		IO::geti()->error("B³¹d otwarcia pliku");
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
	SetWindowText(Controls::geti()->find("editor"),file_content);
	set_selected_1(0, 0);
	SendMessage(Controls::geti()->find("editor"), EM_SCROLLCARET, 0, 0);
	Config::geti()->opened_file = filename;
	stringstream ss;
	ss<<"Odczytano plik: "<<filename;
	IO::geti()->echo(ss.str());
    Controls::geti()->set_text("cmd", filename);
	refresh_text();
	update_title();
	Config::geti()->transposed = 0;
	autoscroll_off();
}

void App::save_file(){
    string new_filename = Controls::geti()->get_text("cmd");
	if(new_filename.length()==0){
		IO::geti()->error("Podaj nazwê pliku!");
		return;
	}
	Config::geti()->opened_file = new_filename;
	fstream plik;
	plik.open(Config::geti()->opened_file.c_str(), fstream::out|fstream::binary);
	if(!plik.good()){
		IO::geti()->error("B³¹d œcie¿ki pliku");
		plik.close();
		return;
	}
	string* str2 = load_text();
	plik.write(str2->c_str(),str_size);
	plik.close();
	delete str2;
	update_title();
	stringstream ss;
	ss<<"Zapisano plik";
	if(Config::geti()->transposed%12!=0){
		ss<<" (Zapis w nowej tonacji: ";
		if(Config::geti()->transposed>0) ss<<"+";
		ss<<Config::geti()->transposed;
		ss<<" !)";
	}
	IO::geti()->echo(ss.str());
}

void App::analyze(){
    int licznik = 0;
    while(skanuj()) licznik++;
    if(licznik==0) IO::geti()->echo("Brak zmian");
    else IO::geti()->echo("Wprowadzono zmiany");
}

void App::transpose(int transponuj){
	if(transponuj==0) return;
	Config::geti()->transposed += transponuj;
    string trans = transpose_string(*load_text(), transponuj);
	SetWindowText(Controls::geti()->find("editor"), trans.c_str());
	refresh_text();
	stringstream ss;
	ss<<"Transpozycja akordów: ";
	if(Config::geti()->transposed>0) ss<<"+";
	ss<<Config::geti()->transposed;
	IO::geti()->echo(ss.str());
}

void App::change_font_size(int change){
	Config::geti()->editor_fontsize += change;
	if(Config::geti()->editor_fontsize<1) Config::geti()->editor_fontsize=1;
	Controls::geti()->set_font("editor", Config::geti()->editor_fontface, Config::geti()->editor_fontsize);
    stringstream ss;
	ss<<"Rozmiar czcionki: "<<Config::geti()->editor_fontsize;
	if(Config::geti()->autoscroll_scaling){
		Config::geti()->autoscroll_interval = Config::geti()->autoscroll_interval * (Config::geti()->editor_fontsize-2*change)/(Config::geti()->editor_fontsize-change);
        Controls::geti()->set_text("autoscroll_interval", Config::geti()->autoscroll_interval);
		ss<<", Interwa³ autoscrolla: "<<Config::geti()->autoscroll_interval<<" ms";
	}
	IO::geti()->echo(ss.str());
}
