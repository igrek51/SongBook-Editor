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

    HMENU menu_bar = CreateMenu();
    HMENU menu1 = CreateMenu();
    HMENU menu2 = CreateMenu();
    HMENU menu3 = CreateMenu();

    AppendMenu(menu1, MF_STRING, 1, "Nowy");
    AppendMenu(menu1, MF_SEPARATOR, 0, NULL);
    AppendMenu(menu1, MF_STRING, 2, "Baza akordów");
    AppendMenu(menu1, MF_SEPARATOR, 0, NULL);
    AppendMenu(menu1, MF_STRING, 3, "Otwórz");
    AppendMenu(menu1, MF_STRING, 4, "Zapisz");
    AppendMenu(menu1, MF_SEPARATOR, 0, NULL);
    AppendMenu(menu1, MF_STRING, 5, "Zamknij");

    AppendMenu(menu2, MF_STRING, 6, "Szukaj");
    AppendMenu(menu2, MF_STRING, 7, "Zamieñ");
    AppendMenu(menu2, MF_SEPARATOR, 0, NULL);
    AppendMenu(menu2, MF_STRING, 8, "Analizuj");

    AppendMenu(menu3, MF_STRING, 9, "Wy³¹cz");
    AppendMenu(menu3, MF_STRING, 10, "Autoscroll z opóŸnieniem");
    AppendMenu(menu3, MF_STRING, 11, "Autoscroll bez opóŸnienia");
    AppendMenu(menu3, MF_SEPARATOR, 0, NULL);
    AppendMenu(menu3, MF_STRING, 12, "Przyspiesz");
    AppendMenu(menu3, MF_STRING, 13, "Zwolnij");

    AppendMenu(menu_bar, MF_POPUP, (UINT_PTR)menu1, "Plik");
    AppendMenu(menu_bar, MF_POPUP, (UINT_PTR)menu2, "Edycja");
    AppendMenu(menu_bar, MF_POPUP, (UINT_PTR)menu3, "Autoscroll");
    AppendMenu(menu_bar, MF_STRING, 4, "Rozwiñ");
    SetMenu(main_window, menu_bar);


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
	IO::geti()->echo("wersja "+version);
}

void App::event_button(WPARAM wParam){
    if(!(wParam>=1 && wParam<=Controls::geti()->controls.size())) return;
    string name = Controls::geti()->get_button_name(wParam);
    if(name.length()==0) return;
	if(name == "new"){ //nowy
		new_file();
	}else if(name == "load"){ //wczytaj
        string str2 = Controls::geti()->get_text("filename_edit");
		if(str2.length()==0){
			IO::geti()->echo("Podaj nazwê pliku.");
		}else{
			open_chords_file(str2);
		}
	}else if(name == "save"){ //zapisz
		save_chords_file();
	}else if(name == "analyze"){ //analizuj
		analyze();
	}else if(name == "replace"){ //zamieñ
		zamien();
	}else if(name == "find"){ //znajdŸ
		znajdz();
	}else if(name == "base"){ //baza akordów
        chordsbase();
	}else if(name == "autoscroll"){ //autoscroll
		autoscroll_switch();
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
		fullscreen_set(!Config::geti()->fullscreen_on);
	}
	if((GetAsyncKeyState(VK_CONTROL)&0x8000)&&!(GetAsyncKeyState(VK_MENU)&0x8000)){ //ctrl
		if(wParam=='A'){
			select_all();
		}else if(wParam=='S'){
			save_chords_file();
		}else if(wParam=='R'){
			refresh_text();
		}else if(wParam=='F'){
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
