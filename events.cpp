#include "app.h"
#include <commctrl.h>
#include <richedit.h>

void App::event_init(HWND *window){
	main_window = *window;
    //parametry
	IO::geti()->get_args();
	//katalog roboczy
	IO::geti()->set_workdir();
	//ustawienia
    Config::geti()->load_from_file();
    //jeœli aplikacja jest ju¿ uruchomiona
    if(IO::geti()->args.size()==2 && instancja2!=NULL){//jeden dodatkowy parametr - nazwa pliku do otwarcia
        IO::geti()->log("Wysy³anie pliku do otwartej instancji aplikacji...");
        for(unsigned int i=0; i<IO::geti()->args.at(1).length(); i++){
            SendMessage(instancja2, 0x0319, 69, (char)IO::geti()->args.at(1)[i]);
        }
        SendMessage(instancja2, 0x0319, 69, 0);
        IO::geti()->log("Zamykanie zbêdnej instancji aplikacji...");
        //SendMessage(main_window, WM_DESTROY, 0, 0);
        DestroyWindow(main_window);
        return;
    }
    //log
    if(Config::geti()->log_enabled){
        IO::geti()->clear_log();
        IO::geti()->log("Hello World...");
        IO::geti()->log_args();
        IO::geti()->log("Wczytano ustawienia z pliku: "+Config::geti()->config_filename);
    }
	//kontrolki
    IO::geti()->log("Tworzenie kontrolek...");
    Controls::geti()->create_static_center("Plik:", 0, 0, 0, 0, "filename");
    Controls::geti()->create_edit("", 0, 0, 0, 0, "filename_edit");

    Controls::geti()->create_button("Nowy", 0, 0, 0, 0, "new");
    Controls::geti()->create_button("Wczytaj", 0, 0, 0, 0, "load");
    Controls::geti()->create_button("Zapisz", 0, 0, 0, 0, "save");
    Controls::geti()->create_button("Baza akordów", 0, 0, 0, 0, "base");

    Controls::geti()->create_edit_center("", 0, 0, 0, 0, "autoscroll_interval");
    Controls::geti()->create_edit_center("", 0, 0, 0, 0, "autoscroll_wait");
    Controls::geti()->create_button("Autoscroll: off", 0, 0, 0, 0, "autoscroll");

    Controls::geti()->create_edit_center(Config::i()->find_edit_placeholder, 0, 0, 0, 0, "find_edit");
    Controls::geti()->create_edit_center(Config::i()->replace_edit_placeholder, 0, 0, 0, 0, "replace_edit");
    Controls::geti()->create_button("ZnajdŸ", 0, 0, 0, 0, "find");
    Controls::geti()->create_button("Zamieñ", 0, 0, 0, 0, "replace");
    Controls::geti()->create_button("Analizuj", 0, 0, 0, 0, "analyze");

    for(int i=0; i<Config::geti()->cmd_outputs_num; i++){
        stringstream ss;
        ss<<"cmd_output"<<i+1;
        Controls::geti()->create_static_center("", 0, 0, 0, 0, ss.str());
    }
    Controls::geti()->create_edit("", 0, 0, 0, 0, "cmd");
	//edytor
    IO::geti()->log("Tworzenie edytora tekstu...");
	if(LoadLibrary("RICHED32.DLL")==NULL){
		IO::geti()->critical_error("B³¹d: brak biblioteki RICHED32.DLL");
		return;
	}
    HWND editor_handle = CreateWindowEx(WS_EX_CLIENTEDGE, RICHEDIT_CLASS, "", WS_CHILD|WS_VISIBLE|WS_VSCROLL|ES_MULTILINE|ES_DISABLENOSCROLL, 0, 0, 0, 0, main_window, (HMENU)100, *hInst, 0);
    Controls::geti()->controls.push_back(new Control(editor_handle, "editor"));
    //tworzenie menu
    IO::geti()->log("Tworzenie menu...");
    Menu* menu_plik = new Menu();
    menu_plik->add_option("Nowy", "new");
    menu_plik->add_separator();
    menu_plik->add_option("Otwórz", "load");
    menu_plik->add_option("Prze³aduj", "reload");
    menu_plik->add_option("Zapisz", "save");
    menu_plik->add_separator();
    menu_plik->add_option("Baza akordów", "base");
    menu_plik->add_separator();
    menu_plik->add_option("Zakoñcz", "exit");
    Menu* menu_edycja = new Menu();
    menu_edycja->add_option("Analizuj", "analyze");
    menu_edycja->add_separator();
    menu_edycja->add_option("Szukaj", "find");
    menu_edycja->add_option("Zamieñ", "replace");
    menu_edycja->add_option("Szybka zamiana", "quick_replace");
    menu_edycja->add_separator();
    menu_edycja->add_option("Usuñ akordy", "remove_chords");
    menu_edycja->add_option("Usuñ dodatkowe wersje", "remove_alt");
    Menu* menu_widok = new Menu();
    menu_widok->add_option("Zwiêksz czcionkê", "font++");
    menu_widok->add_option("Zmniejsz czcionkê", "font--");
    menu_widok->add_separator();
    menu_widok->add_option("Formatuj tekst", "format_text");
    menu_widok->add_option("Przewiñ na pocz¹tek", "scroll_to_begin");
    menu_widok->add_option("Przewiñ na koniec", "scroll_to_end");
    menu_widok->add_separator();
    menu_widok->add_option("Pe³ny ekran", "fullscreen");
    Menu* menu_autoscroll = new Menu();
    menu_autoscroll->add_option("W³¹cz z opóŸnieniem", "autoscroll_wait");
    menu_autoscroll->add_option("W³¹cz bez opóŸnienia", "autoscroll_nowait");
    menu_autoscroll->add_option("Wy³¹cz", "autoscroll_off");
    menu_autoscroll->add_separator();
    menu_autoscroll->add_option("Zwolnij przewijanie", "autoscroll_slower");
    menu_autoscroll->add_option("Przyspiesz przewijanie", "autoscroll_faster");
    Menu* menu_transpozycja = new Menu();
    menu_transpozycja->add_option("Transponuj 5 pó³tonów w górê", "transpose+5");
    menu_transpozycja->add_option("Transponuj 1 pó³ton w górê", "transpose++");
    menu_transpozycja->add_separator();
    menu_transpozycja->add_option("Transponuj 1 pó³ton w dó³", "transpose--");
    menu_transpozycja->add_option("Transponuj 5 pó³tonów w dó³", "transpose-5");
    menu_transpozycja->add_separator();
    menu_transpozycja->add_option("Oryginalna tonacja", "transpose0");
    menu_transpozycja->add_separator();
    menu_transpozycja->add_option("Dodaj alternatywn¹ tonacjê", "alt");
    Menu* menu_ustawienia = new Menu();
    menu_ustawienia->add_option("Plik konfiguracyjny", "config");
    menu_ustawienia->add_option("Wiersz poleceñ", "cmd_toggle");
    menu_ustawienia->add_option("Dziennik zdarzeñ", "log");
    Menu* menu_pomoc = new Menu();
    menu_pomoc->add_option("Polecenia i skróty klawiszowe", "help");
    menu_pomoc->add_option("O programie", "info");
    //g³ówny pasek menu
    Menu* menu_bar = new Menu();
    menu_bar->add_menu(menu_plik, "Plik");
    menu_bar->add_menu(menu_edycja, "Edycja");
    menu_bar->add_menu(menu_widok, "Widok");
    menu_bar->add_option("Pasek narzêdzi", "toolbar_toggle");
    menu_bar->add_menu(menu_autoscroll, "Autoscroll");
    menu_bar->add_menu(menu_transpozycja, "Transpozycja");
    menu_bar->add_menu(menu_ustawienia, "Ustawienia");
    menu_bar->add_menu(menu_pomoc, "Pomoc");
    SetMenu(main_window, menu_bar->handle);
    //autoscroll edits
    IO::geti()->log("Wype³nianie kontrolek, zmiana czcionek...");
    Controls::geti()->set_text("autoscroll_interval", Config::geti()->autoscroll_interval);
    Controls::geti()->set_text("autoscroll_wait", Config::geti()->autoscroll_wait);
    //czcionki
    controls_fonts_set();
	Controls::geti()->set_focus("editor");
	//subclassing
    IO::geti()->log("Subclassing...");
    for(unsigned int i=0; i<Controls::geti()->controls.size(); i++){
        subclass(Controls::geti()->controls.at(i));
    }
	//pasek schowany przy starcie
	if(!Config::geti()->toolbar_show){
		toolbar_switch(0);
	}
	//okno na po³owie ekranu
	if(Config::geti()->halfscreen==1){
        IO::geti()->log("Rozmieszczanie okna na po³owie ekranu...");
		HDC screen = GetDC(GetDesktopWindow());
		int screen_w = GetDeviceCaps(screen, HORZRES);
		int screen_h = GetDeviceCaps(screen, VERTRES);
		DeleteDC(screen);
		SetWindowPos(main_window, HWND_TOP, -8, 0, screen_w/2, screen_h-34, 0);
        //  TODO
    }
    event_resize();
    //drag & drop
    IO::geti()->log("Uaktywanianie funkcji drag & drop...");
	DragAcceptFiles(main_window, true);
	Controls::geti()->set_text("editor", "");
	//wczytanie pliku zadanego parametrem
	if(IO::geti()->args.size()==2){ //jeden dodatkowy parametr - nazwa pliku do otwarcia
		open_chords_file(IO::geti()->args.at(1));
	}
	update_title();
    //baza akordów na start (jeœli nie by³ otwierany wybrany plik)
    if(Config::geti()->chordsbase_on_startup && IO::geti()->args.size()<=1){
        chordsbase();
    }
    refresh_text();
	IO::geti()->echo("Gotów do pracy - wersja "+version);
}

void App::event_button(WPARAM wParam){
    string name = "";
    if(wParam>=1 && wParam<=Controls::geti()->controls.size()){
        name = Controls::geti()->get_button_name(wParam);
    }else{
        name = Controls::geti()->get_menu_name(wParam);
    }
    if(name.length()==0) return;
	if(name == "new"){ //nowy
		new_file();
	}else if(name == "load"){ //wczytaj
        if(!Config::geti()->toolbar_show){
            toolbar_switch(1);
        }
        string str2 = Controls::geti()->get_text("filename_edit");
		if(str2.length()==0){
			IO::geti()->echo("Podaj nazwê pliku.");
		}else{
			open_chords_file(str2);
		}
	}else if(name == "save"){ //zapisz
        if(!Config::geti()->toolbar_show){
            toolbar_switch(1);
        }
		save_chords_file();
	}else if(name == "analyze"){ //analizuj
		analyze();
	}else if(name == "replace"){ //zamieñ
        if(!Config::geti()->toolbar_show){
            toolbar_switch(1);
            return;
        }
		zamien();
	}else if(name == "find"){ //znajdŸ
        if(!Config::geti()->toolbar_show){
            toolbar_switch(1);
            return;
        }
		znajdz();
	}else if(name == "base"){ //baza akordów
        chordsbase();
	}else if(name == "autoscroll"){ //autoscroll
		autoscroll_switch();
	}else if(name == "reload"){
        if(!Config::geti()->toolbar_show){
            toolbar_switch(1);
        }
        open_chords_file(Config::geti()->opened_file);
    }else if(name == "config"){
        ShellExecute(0,"open",Config::geti()->config_filename.c_str(),"",0,SW_SHOW);
    }else if(name == "quick_replace"){
        quick_replace();
    }else if(name == "remove_chords"){
        usun_akordy();
    }else if(name == "remove_alt"){
        usun_wersje();
    }else if(name == "font++"){
        change_font_size(+1);
    }else if(name == "font--"){
        change_font_size(-1);
    }else if(name == "format_text"){
        refresh_text();
    }else if(name == "fullscreen"){
        fullscreen_toggle();
    }else if(name == "autoscroll_wait"){
        autoscroll_on();
    }else if(name == "autoscroll_nowait"){
        autoscroll_nowait();
    }else if(name == "autoscroll_off"){
        autoscroll_off();
        IO::geti()->echo("Autoscroll wy³¹czony");
    }else if(name == "autoscroll_slower"){
        autoscroll_nowait(+Config::geti()->autoscroll_interval*0.25);
    }else if(name == "autoscroll_faster"){
        autoscroll_nowait(-Config::geti()->autoscroll_interval*0.2);
    }else if(name == "transpose+5"){
        transpose(+5);
    }else if(name == "transpose++"){
        transpose(+1);
    }else if(name == "transpose--"){
        transpose(-1);
    }else if(name == "transpose-5"){
        transpose(-5);
    }else if(name == "transpose0"){
        transpose(-Config::geti()->transposed);
    }else if(name == "alt"){
        dodaj_alternatywne();
    }else if(name == "log"){
        ShellExecute(0, "open", Config::geti()->log_filename.c_str(), "", 0, SW_SHOW);
    }else if(name == "help"){
        show_help();
    }else if(name == "info"){
        stringstream ss;
        ss<<Config::geti()->program_name<<endl;
        ss<<"wersja "<<version<<endl;
        IO::geti()->message_box("O programie",ss.str());
    }else if(name == "cmd_toggle"){
        cmd_switch();
    }else if(name == "toolbar_toggle"){
        toolbar_switch();
    }else if(name == "scroll_to_begin"){
        set_scroll(0);
    }else if(name == "scroll_to_end"){
        SendMessage(Controls::geti()->find("editor"), WM_VSCROLL, SB_BOTTOM, 0);
    }else if(name == "exit"){
        DestroyWindow(main_window);
    }else{
        IO::geti()->error("Zdarzenie nie zosta³o obs³u¿one: "+name);
    }
}

void App::event_dropfiles(string filename){
	if(file_exists(filename)){
		open_chords_file(filename);
		Controls::geti()->set_focus("editor");
	}else if(dir_exists(filename)){
		new_file();
		Config::geti()->opened_file = filename;
        if(Config::geti()->opened_file[Config::geti()->opened_file.length()-1]!='\\'){
            Config::geti()->opened_file+="\\";
        }
        Controls::geti()->set_text("filename_edit", Config::geti()->opened_file);
		Controls::geti()->set_focus("filename_edit");
		IO::geti()->echo("Nowy plik w folderze: \""+filename+"\"");
	}else{
		IO::geti()->error("nieprawid³owa œcie¿ka: \""+filename+"\"");
	}
	SetForegroundWindow(main_window);
}

void App::event_resize(){
    IO::geti()->log("Resize okna - Odœwie¿anie uk³adu kontrolek...");
    if(!Controls::geti()->exists("cmd_output1")) return;
    if(!Controls::geti()->exists("editor")) return;
    int ch = Config::geti()->control_height;
	RECT wnd_rect;
	GetClientRect(main_window, &wnd_rect);
	int w = wnd_rect.right-wnd_rect.left;
	int h = wnd_rect.bottom-wnd_rect.top;
    Config::geti()->window_w = w;
    Config::geti()->window_h = h;
    //rozmieszczenie kontrolek
    int editor_h = h-ch;
    int editor_y = 0;
    if(Config::geti()->cmd_show){
        editor_h -= ch*Config::geti()->cmd_outputs_num;
    }

	if(Config::geti()->toolbar_show){
        editor_h -= ch*3;
        editor_y += ch*3;
    }
    Controls::i()->resize("editor", 0,editor_y,w,editor_h);

    Controls::i()->resize("filename", 0,0,Config::geti()->static_filename_width,ch);
    Controls::i()->resize("filename_edit", Config::geti()->static_filename_width,0,w-Config::geti()->static_filename_width,ch);
    //1. rz¹d
    Controls::i()->resize("new", w*0/7,ch*1,w/7,ch);
    Controls::i()->resize("load", w*1/7,ch*1,w/7,ch);
    Controls::i()->resize("save", w*2/7,ch*1,w/7,ch);
    Controls::i()->resize("base", w*3/7,ch*1,w/7,ch);
    Controls::i()->resize("autoscroll_interval", w*4/7,ch*1,w/7,ch);
    Controls::i()->resize("autoscroll_wait", w*5/7,ch*1,w/7,ch);
    Controls::i()->resize("autoscroll", w*6/7,ch*1,w/7,ch);
	//2. rz¹d
    Controls::i()->resize("find_edit", w*0/7,ch*2,w*2/7,ch);
    Controls::i()->resize("replace_edit", w*2/7,ch*2,w*2/7,ch);
    Controls::i()->resize("find", w*4/7,ch*2,w/7,ch);
    Controls::i()->resize("replace", w*5/7,ch*2,w/7,ch);
    Controls::i()->resize("analyze", w*6/7,ch*2,w/7,ch);
    //konsola
    if(Config::geti()->cmd_show){
        for(int i=0; i<Config::geti()->cmd_outputs_num; i++){
            stringstream ss;
            ss<<"cmd_output"<<i+1;
            Controls::i()->resize(ss.str(), Config::geti()->cmd_outputs_space,h-ch*(i+2),w-Config::geti()->cmd_outputs_space,ch);
        }
        Controls::i()->resize("cmd", 0,h-ch,w,ch);
    }else{
        Controls::i()->resize("cmd_output1", 0,h-ch,w,ch);
    }
}

void App::event_screensave(){
	IO::geti()->log("Screensaver stop");
	mouse_event(MOUSEEVENTF_MOVE,1,0,0,0);
	mouse_event(MOUSEEVENTF_MOVE,-1,0,0,0);
}

void App::event_timer(){
	autoscroll_exec();
}

void App::event_appcommand(WPARAM wParam, LPARAM lParam){
    if(wParam==69){
        char newc = (char)lParam;
        if(newc==0){ //koniec przesy³ania nazwy pliku
            stringstream ss;
            ss<<"Otwieranie pliku z zewnêtrznego polecenia: "<<Config::geti()->file_to_open;
            IO::geti()->log(ss.str());
            open_chords_file(Config::geti()->file_to_open);
            Config::geti()->file_to_open = "";
            SetForegroundWindow(main_window);
            return;
        }
        Config::geti()->file_to_open += newc;
    }
}

void App::event_syskeydown(WPARAM wParam){
	if(wParam==VK_F10){
		event_keydown(wParam);
	}
}

void App::event_keydown(WPARAM wParam){
	if(wParam==VK_ESCAPE){
		Controls::geti()->set_focus("editor");
	}
	if(wParam==VK_F1){
		set_scroll(0);
	}else if(wParam==VK_F2){
		change_scroll(-35);
	}else if(wParam==VK_F3){
		change_scroll(+35);
	}else if(wParam==VK_F5){
		autoscroll_nowait(+Config::geti()->autoscroll_interval*0.25);
	}else if(wParam==VK_F6){
		autoscroll_nowait(-Config::geti()->autoscroll_interval*0.2);
	}else if(wParam==VK_F7){
		autoscroll_switch();
	}else if(wParam==VK_F8){
		if(Config::geti()->autoscroll){
			autoscroll_off();
			IO::geti()->echo("Autoscroll wy³¹czony");
		}else{
			autoscroll_nowait();
		}
	}else if(wParam==VK_F9){
		toolbar_switch();
	}else if(wParam==VK_F11){
        fullscreen_toggle();
	}
	if((GetAsyncKeyState(VK_CONTROL)&0x8000)&&!(GetAsyncKeyState(VK_MENU)&0x8000)){ //ctrl
		if(wParam=='A'){
			select_all();
		}else if(wParam=='S'){
			save_chords_file();
		}else if(wParam=='R'){
			refresh_text();
		}else if(wParam=='F'){
            if(!Config::geti()->toolbar_show){
                toolbar_switch(1);
            }
			Controls::geti()->set_focus("find_edit");
		}else if(wParam==VK_ADD){ // +
			change_font_size(+1);
		}else if(wParam==VK_SUBTRACT){ // -
			change_font_size(-1);
		}else if(wParam==VK_OEM_3){ // `
            cmd_switch();
            if(Config::geti()->cmd_show){
                Controls::geti()->set_focus("cmd");
                Controls::geti()->select_all("cmd");
            }else{
                Controls::geti()->set_focus("editor");
            }
		}else if(wParam==VK_LEFT){
			transpose(-1);
		}else if(wParam==VK_RIGHT){
			transpose(+1);
		}else if(wParam=='0'||wParam==VK_NUMPAD0){
			transpose(-Config::geti()->transposed);
		}
	}
}
