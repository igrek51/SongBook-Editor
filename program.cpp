#include "app.h"
#include "strings.h"
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

void App::chordsbase(){
    IO::geti()->log("Otwieranie bazy akordów...");
    if(Config::geti()->songs_dir.length()>0){
        ShellExecute(0, "open", Config::geti()->songs_dir.c_str(), "", "", SW_SHOW);
    }
}

void App::quick_replace(){
    string selected = get_selected_text();
	selected = trim_spaces(trim_1crlf(selected));
    if(selected.length()==0){
        IO::geti()->error("Pusty tekst do zamiany.");
        return;
    }
    get_selected_1(last_sel_start, last_sel_end);
    set_selected_1(last_sel_end, last_sel_end); //brak zaznaczenia - replace obejmuje ca³oœæ
    Controls::geti()->set_text("find_edit", selected);
    Controls::geti()->set_text("replace_edit", selected);
    Controls::geti()->select_all("replace_edit");
    Controls::geti()->set_focus("replace_edit");
}


void App::new_file(){
	SetWindowText(Controls::geti()->find("editor"), "");
	refresh_text();
    //usuniêcie nazwy samego pliku
    while(Config::geti()->opened_file.length()>0 && Config::geti()->opened_file[Config::geti()->opened_file.length()-1]!='\\'){
        Config::geti()->opened_file = string_cutfromend(Config::geti()->opened_file, 1);
    }
    Controls::i()->set_text("cmd", Config::geti()->opened_file);
	Config::geti()->opened_file = "";
	update_title();
	IO::geti()->echo("Nowy plik");
}

void App::open_chords_file(string filename){
	char *file_content = open_file(filename);
    if(file_content==NULL) return;
	SetWindowText(Controls::geti()->find("editor"), file_content);
    delete[] file_content;
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

void App::save_chords_file(){
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
	plik.write(str2->c_str(), str2->length());
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
    while(skanuj_1()) licznik++;
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
