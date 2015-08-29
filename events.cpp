#include "app.h"
#include <commctrl.h>
#include <richedit.h>

void App::event_init(HWND *window){
	main_window = *window;
    IO::geti()->log("Hello World...");
    //parametry
	System::geti()->get_args();
	//katalog roboczy
	IO::geti()->set_workdir();
	//ustawienia
    Config::geti()->load_from_file();
	//log
    if(!Config::geti()->log_enabled) IO::geti()->delete_log();
    //jeœli aplikacja jest ju¿ uruchomiona
    if(IO::geti()->args.size()==2 && instancja2!=NULL){//jeden dodatkowy parametr - nazwa pliku do otwarcia
        IO::geti()->log("Wysy³anie pliku do otwartej instancji aplikacji...");
        for(unsigned int i=0; i<IO::geti()->args.at(1).length(); i++){
            SendMessage(instancja2, 0x0319, 69, (char)IO::geti()->args.at(1)[i]);
        }
        SendMessage(instancja2, 0x0319, 69, 0);
        IO::geti()->log("Zamykanie zbêdnej instancji aplikacji...");
        DestroyWindow(main_window);
        return;
    }
	//kontrolki
    IO::geti()->log("Tworzenie kontrolek...");
    int buttonw = Config::geti()->window_w/7;
    int control_h = Config::geti()->control_height;
    int x_paint=0;
    int y_paint=40;
    int w = Config::geti()->window_w;
    int h = Config::geti()->window_h;
    Controls::geti()->create_static_center("", 0, 0, 0, 0, "statusbar");
    Controls::geti()->create_edit("", 0, 0, 0, 0, "cmd");

    Controls::geti()->create_button("Nowy", x_paint, y_paint, buttonw, 20, "new");
    x_paint+=buttonw;
    Controls::geti()->create_button("Wczytaj", x_paint, y_paint, buttonw, 20, "load");
    x_paint+=buttonw;
    Controls::geti()->create_button("Zapisz", x_paint, y_paint, buttonw, 20, "save");
    x_paint+=buttonw;
    Controls::geti()->create_button("Baza akordów", x_paint, y_paint, buttonw, 20, "base");
    x_paint+=buttonw;

    Controls::geti()->create_edit_center("", x_paint, y_paint, buttonw, 20, "autoscroll_interval");
    x_paint+=buttonw;
    Controls::geti()->create_edit_center("", x_paint, y_paint, buttonw, 20, "autoscroll_wait");
    x_paint+=buttonw;
    Controls::geti()->create_button("Autoscroll: off", x_paint, y_paint, buttonw, 20, "autoscroll");
    x_paint+=buttonw;

    x_paint = 0; y_paint+=20;
    Controls::geti()->create_edit_center("", x_paint, y_paint, buttonw*2, 20, "find_edit");
    x_paint+=buttonw*2;
    Controls::geti()->create_edit_center("", x_paint, y_paint, buttonw*2, 20, "replace_edit");
    x_paint+=buttonw*2;
    Controls::geti()->create_button("ZnajdŸ", x_paint, y_paint, buttonw, 20, "find");
    x_paint+=buttonw;
    Controls::geti()->create_button("Zamieñ", x_paint, y_paint, buttonw, 20, "replace");
    x_paint+=buttonw;
    Controls::geti()->create_button("Analizuj", x_paint, y_paint, buttonw, 20, "analyze");
    x_paint+=buttonw;

    Controls::geti()->create_button("^", w-20, 0, 20, control_h, "toolbar_toggle");
	//edytor
    IO::geti()->log("Tworzenie edytora tekstu...");
	if(LoadLibrary("RICHED32.DLL")==NULL){
		IO::geti()->critical_error("B³¹d: brak biblioteki RICHED32.DLL");
		return;
	}
    HWND editor_handle = CreateWindowEx(WS_EX_CLIENTEDGE, RICHEDIT_CLASS, "", WS_CHILD|WS_VISIBLE|WS_VSCROLL|ES_MULTILINE|ES_DISABLENOSCROLL, 0, 80, w, h-80, main_window, (HMENU)100, *hInst, 0);
    Controls::geti()->controls.push_back(new Control(editor_handle, "editor"));
	//autoscroll edits
    IO::geti()->log("Wype³nianie kontrolek, zmiana czcionek...");
    Controls::geti()->set_text("autoscroll_interval", Config::geti()->autoscroll_interval);
    Controls::geti()->set_text("autoscroll_wait", Config::geti()->autoscroll_wait);
	//czcionki
    controls_fonts_set();
	SetFocus(Controls::geti()->find("editor"));
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
    }else{
        event_resize();
    }
    //drag & drop
    IO::geti()->log("Uaktywanianie funkcji drag & drop...");
	DragAcceptFiles(main_window, true);
	Controls::geti()->set_text("editor", "");
	//wczytanie pliku zadanego parametrem
	if(IO::geti()->args.size()==2){ //jeden dodatkowy parametr - nazwa pliku do otwarcia
		open_file(IO::geti()->args.at(1));
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
	if(name == "nowy"){ //nowy
		new_file();
	}else if(name == "load"){ //wczytaj
		char *str2 = new char[512];
		GetWindowText(Controls::geti()->find("cmd"), str2, 512);
		if(strlen(str2)==0){
			IO::geti()->echo("Podaj nazwê pliku.");
		}else{
			open_file(str2);
		}
		delete[] str2;
	}else if(name == "save"){ //zapisz
		save_file();
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
	}else if(name == "toolbar_toggle"){ //schowanie paska
		if(Config::geti()->fullscreen_on) fullscreen_set(false);
        else toolbar_switch();
	}
}

void App::event_dropfiles(string filename){
	if(file_exists(filename)){
		open_file(filename);
		SetFocus(Controls::geti()->find("editor"));
	}else if(dir_exists(filename)){
		new_file();
		Config::geti()->opened_file = filename;
        if(Config::geti()->opened_file[Config::geti()->opened_file.length()-1]!='\\'){
            Config::geti()->opened_file+="\\";
        }
        Controls::geti()->set_text("cmd", Config::geti()->opened_file);
		SetFocus(Controls::geti()->find("cmd"));
		IO::geti()->echo("Nowy plik w folderze: \""+filename+"\"");
	}else{
		IO::geti()->error("nieprawid³owa œcie¿ka: \""+filename+"\"");
	}
	SetForegroundWindow(main_window);
}

void App::event_resize(){
    IO::geti()->log("Resize okna - Odœwie¿anie uk³adu kontrolek...");
    int ch = Config::geti()->control_height;
    int toolbar_b_w = Config::geti()->toolbar_button_width;
	RECT wnd_rect;
	GetClientRect(main_window, &wnd_rect);
	int w = wnd_rect.right-wnd_rect.left;
	int h = wnd_rect.bottom-wnd_rect.top;
    Config::geti()->window_w = w;
    Config::geti()->window_h = h;
    //rozmieszczenie kontrolek
	if(Config::geti()->toolbar_show){
        Controls::i()->resize("editor", 0,ch*3,w,h-ch*4);
	}else{
        Controls::i()->resize("editor", 0,0,w,h-ch);
	}
    Controls::i()->resize("cmd", 0,0,w,ch);
    Controls::i()->resize("statusbar", 0,h-ch,w-toolbar_b_w,ch);
    Controls::i()->resize("toolbar_toggle", w-toolbar_b_w,h-ch,toolbar_b_w,ch);
    //resize panelu - 1. rz¹d
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
    Controls::i()->resize("find", w*5/7,ch*2,w/7,ch);
    Controls::i()->resize("replace", w*4/7,ch*2,w/7,ch);
    Controls::i()->resize("analyze", w*6/7,ch*2,w/7,ch);
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
            open_file(Config::geti()->file_to_open);
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
		SetFocus(Controls::geti()->find("editor"));
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
			save_file();
		}else if(wParam=='R'){
			refresh_text();
		}else if(wParam=='F'){
			SetFocus(Controls::geti()->find("find_edit"));
		}else if(wParam==VK_ADD){ // +
			change_font_size(+1);
		}else if(wParam==VK_SUBTRACT){ // -
			change_font_size(-1);
		}else if(wParam==VK_OEM_3){ // `
			SetFocus(Controls::geti()->find("cmd"));
			SendMessage(Controls::geti()->find("cmd"), EM_SETSEL, 0, -1);
		}else if(wParam==VK_LEFT){
			transpose(-1);
		}else if(wParam==VK_RIGHT){
			transpose(+1);
		}else if(wParam=='0'||wParam==VK_NUMPAD0){
			transpose(-Config::geti()->transposed);
		}
	}
}
