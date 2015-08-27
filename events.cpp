#include "app.h"
#include <commctrl.h>
#include <richedit.h>

void App::event_init(HWND *window){
	main_window = *window;
    //parametry
	System::geti()->get_args();
    //jeœli aplikacja jest ju¿ uruchomiona
    if(IO::geti()->args.size()==2 && instancja2!=NULL){//jeden dodatkowy parametr - nazwa pliku do otwarcia
        for(unsigned int i=0; i<IO::geti()->args.at(1).length(); i++){
            SendMessage(instancja2, 0x0319, 69, (char)IO::geti()->args.at(1)[i]);
        }
        SendMessage(instancja2, 0x0319, 69, 0);
        DestroyWindow(main_window);
        return;
    }
	//katalog roboczy
	IO::geti()->set_workdir();
	//ustawienia
    Config::geti()->load_from_file();
	//log
	IO::geti()->clear_log();
	IO::geti()->log("Hello World...");
	//kontrolki
    int buttonw = Config::geti()->window_w/7;
    int control_h = Config::geti()->control_height;
    int x_paint=0;
    int y_paint=40;
    int w = Config::geti()->window_w;
    int h = Config::geti()->window_h;
    Controls::geti()->create_static_center("", 0, 0, w-20, h, "statusbar");
    Controls::geti()->create_edit("", 0, control_h, w, control_h, "cmd");

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
    Controls::geti()->create_edit("", x_paint, y_paint, buttonw*2, 20, "find_edit");
    x_paint+=buttonw*2;
    Controls::geti()->create_edit("", x_paint, y_paint, buttonw*2, 20, "replace_edit");
    x_paint+=buttonw*2;
    Controls::geti()->create_button("ZnajdŸ", x_paint, y_paint, buttonw, 20, "find");
    x_paint+=buttonw;
    Controls::geti()->create_button("Zamieñ", x_paint, y_paint, buttonw, 20, "replace");
    x_paint+=buttonw;
    Controls::geti()->create_button("Analizuj", x_paint, y_paint, buttonw, 20, "analyze");
    x_paint+=buttonw;

    Controls::geti()->create_button("^", w-control_h, 0, 20, control_h, "toolbar_toggle");
	//edytor
	if(LoadLibrary("RICHED32.DLL")==NULL){
		IO::geti()->error("B³¹d: brak biblioteki RICHED32.DLL");
		return;
	}
    HWND editor_handle = CreateWindowEx(WS_EX_CLIENTEDGE, RICHEDIT_CLASS, "", WS_CHILD|WS_VISIBLE|WS_VSCROLL|ES_MULTILINE|ES_DISABLENOSCROLL, 0, 80, w, h-80, main_window, (HMENU)100, *hInst, 0);
    Controls::geti()->controls.push_back(new Control(editor_handle, "editor"));
	//autoscroll edits
    Controls::geti()->set_text("autoscroll_interval", Config::geti()->autoscroll_interval);
    Controls::geti()->set_text("autoscroll_wait", Config::geti()->autoscroll_wait);
	//drag & drop
	DragAcceptFiles(main_window, true);
	Controls::geti()->set_text("editor", "");
	//wczytanie pliku zadanego parametrem
	if(IO::geti()->args.size()==2){ //jeden dodatkowy parametr - nazwa pliku do otwarcia
		open_file(IO::geti()->args.at(1));
	}
	update_title();
	//czcionki
    for(unsigned int i=0; i<Controls::geti()->controls.size(); i++){
        string fontface = Config::geti()->buttons_fontface;
        int fontsize = Config::geti()->buttons_fontsize;
        if(Controls::geti()->controls.at(i)->name == "editor"){
            fontface = Config::geti()->editor_fontface;
            fontsize = Config::geti()->editor_fontsize;
        }
        Controls::geti()->set_font(Controls::geti()->controls.at(i)->handle, fontface, fontsize);
    }
	SetFocus(Controls::geti()->find("editor"));
	//subclassing
    for(unsigned int i=0; i<Controls::geti()->controls.size(); i++){
        subclass(Controls::geti()->controls.at(i));
    }
	//pasek schowany przy starcie
	if(!Config::geti()->toolbar_show){
		pasek_switch(0);
	}
	//okno na po³owie ekranu
	if(Config::geti()->halfscreen==1){
		HDC screen = GetDC(GetDesktopWindow());
		int screen_w = GetDeviceCaps(screen, HORZRES);
		int screen_h = GetDeviceCaps(screen, VERTRES);
		DeleteDC(screen);
		SetWindowPos(main_window, HWND_TOP, 0, 0, screen_w/2, screen_h-30, 0);
    }
    //baza akordów na start (jeœli nie by³ otwierany wybrany plik)
    if(Config::geti()->chordsbase_on_startup && IO::geti()->args.size()<=1){
        chordsbase();
    }
	IO::geti()->echo("wersja "+version);
}

void App::event_button(WPARAM wParam){
    //  TODO
    /*
    string name = get_button_name(wParam);
	if(name == "nowy"){ //nowy
		new_file();
	}
	if(wParam==12){ //wczytaj
		char *str2 = new char[512];
		GetWindowText(hctrl[0],str2,512);
		if(strlen(str2)==0){
			echo("Podaj nazwê pliku");
		}else{
			open_file(str2);
		}
		delete[] str2;
	}
	if(wParam==2){ //zapisz
		save_file();
	}
    if(wParam==3){ //analizuj
		int licznik=0;
		while(skanuj()) licznik++;
		if(licznik==0) echo("Brak zmian");
		else echo("Wprowadzono zmiany");
	}
	if(wParam==7){ //zamieñ
		zamien();
	}
	if(wParam==13){ //znajdŸ
		znajdz();
	}
    if(wParam==9){ //baza akordów
        chordsbase();
	}
	if(wParam==14){ //autoscroll
		autoscroll_switch();
	}
	if(wParam==16){ //schowanie paska
		if(fullscreen_on) fullscreen_set(false);
        else pasek_switch();
	}
    */
}

void App::event_dropfiles(string filename){
    /*
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
    */
}

void App::event_resize(){
    /*
    int control_h = 22;
	RECT wnd_rect;
	GetClientRect(main_window, &wnd_rect);
	int w = wnd_rect.right-wnd_rect.left;
	int h = wnd_rect.bottom-wnd_rect.top;
    Config::geti()->window_w = w;
    Config::geti()->window_h = h;
	if(Config::geti()->toolbar_show){
        SetWindowPos(Controls::geti()->find("editor"),HWND_TOP,0,control_h*4,w,h-control_h*4,0);
	}else{
        SetWindowPos(Controls::geti()->find("editor"),HWND_TOP,0,control_h,w,h-control_h,0);
	}
    SetWindowPos(Controls::geti()->find("statusbar"),HWND_TOP,0,0,w-20,control_h,SWP_NOMOVE);
    SetWindowPos(Controls::geti()->find("cmd"),HWND_TOP,0,0,w,control_h,SWP_NOMOVE);
	SetWindowPos(Controls::geti()->find("toolbar_toggle"),HWND_TOP,w-20,0,0,0,SWP_NOSIZE);
    //resize panelu - 1. rz¹d
    SetWindowPos(Controls::geti()->find("new"),HWND_TOP,w*0/7,control_h*2,w/7,control_h,0);
    SetWindowPos(Controls::geti()->find("load"),HWND_TOP,w*1/7,control_h*2,w/7,control_h,0);
    SetWindowPos(Controls::geti()->find("save"),HWND_TOP,w*2/7,control_h*2,w/7,control_h,0);
    SetWindowPos(Controls::geti()->find("base"),HWND_TOP,w*3/7,control_h*2,w/7,control_h,0);
    SetWindowPos(Controls::geti()->find("autoscroll_interval"),HWND_TOP,w*4/7,control_h*2,w/7,control_h,0);
    SetWindowPos(Controls::geti()->find("autoscroll_wait"),HWND_TOP,w*5/7,control_h*2,w/7,control_h,0);
    SetWindowPos(Controls::geti()->find("autoscroll"),HWND_TOP,w*6/7,control_h*2,w/7,control_h,0);
	//2. rz¹d
    SetWindowPos(Controls::geti()->find("find_edit"),HWND_TOP,w*0/7,control_h*3,w*2/7,control_h,0);
    SetWindowPos(Controls::geti()->find("replace_edit"),HWND_TOP,w*2/7,control_h*3,w*2/7,control_h,0);
    SetWindowPos(Controls::geti()->find("find"),HWND_TOP,w*5/7,control_h*3,w/7,control_h,0);
    SetWindowPos(Controls::geti()->find("replace"),HWND_TOP,w*4/7,control_h*3,w/7,control_h,0);
    SetWindowPos(Controls::geti()->find("analyze"),HWND_TOP,w*6/7,control_h*3,w/7,control_h,0);
    */
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
	if(wParam==VK_RETURN||wParam==VK_SPACE||wParam==VK_ESCAPE){
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
		pasek_switch();
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
			//change_font_size(+1);
		}else if(wParam==VK_SUBTRACT){ // -
			//change_font_size(-1);
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
