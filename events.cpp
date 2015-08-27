#include "app.h"
#include <commctrl.h>
#include <richedit.h>

void App::event_create(HWND *window){
	main_window = *window;
	//parametry
	System::geti()->get_args();
    //jeœli aplikacja jest ju¿ uruchomiona
    if(IO::geti()->args.size()==2 && instancja2!=NULL){//jeden dodatkowy parametr - nazwa pliku do otwarcia
        for(unsigned int i=0; i<IO::geti()->args.at(1).length(); i++){
            SendMessage(instancja2, 0x0319, 69, (char)IO::geti()->args.at(1)[i]);
        }
        SendMessage(instancja2, 0x0319, 69, 0);
        DestroyWindow(hwnd);
        return;
    }
	//katalog roboczy
	set_workdir();
	//ustawienia
    Config::geti()->load_from_file();
	transposed = 0;
	autoscroll = false;
	opened_file = "";
	for(int i=0; i<9; i++) tekst_wstaw[i]="";
    file_to_open = "";
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

    Controls::geti()->create_button("^", w-control_h, 0, 20, control_h, "toolbar_hide");
	//edytor
	if(LoadLibrary("RICHED32.DLL")==NULL){
		IO::geti()->error("B³¹d: brak biblioteki RICHED32.DLL");
		return;
	}
    HWND editor_handle = CreateWindowEx(WS_EX_CLIENTEDGE, RICHEDIT_CLASS, "", WS_CHILD|WS_VISIBLE|WS_VSCROLL|ES_MULTILINE|ES_DISABLENOSCROLL, 0, 80, w, h-80, main_window, (HMENU)100, *hInst, 0);
    controls.push_back(new Control(editor_handle, "editor"));
	//autoscroll edits
    Controls::geti()->set_text("autoscroll_interval", Config::geti()->autoscroll_interval);
    Controls::geti()->set_text("autoscroll_wait", Config::geti()->autoscroll_wait);
	//drag & drop
	DragAcceptFiles(hwnd,true);
	SetWindowText(hctrl[2],"");
	//wczytanie pliku zadanego parametrem
	if(argc==2){ //jeden dodatkowy parametr - nazwa pliku do otwarcia
		open_file(argv[1]);
	}
	if(is_arg("--fo")){ //file open
		for(int i=0; i<argc-1; i++){
			if(argv[i]=="--fo"){
				if(argv[i+1].length()<3) break;
				open_file(argv[i+1]);
				break;
			}
		}
	}
	update_title();
	//czcionki
	fontsize=config_fontsize1;
    fontface=config_fontface;
	for(int i=0; i<ctrls_num; i++){
		if(i==2) continue;
		change_font(hctrl[i]);
	}
	fontsize=config_fontsize2;
	change_font(hctrl[2]);
	SetFocus(hctrl[2]);
	//subclassing
	subclass(0);
	subclass(2);
	subclass(9);
	subclass(10);
	subclass(19);
	subclass(20);
	subclass(22);
	//pasek schowany przy starcie
	pasek_shown = true;
	fullscreen_on = false;
	if(Config::toolbar_show==0){
		pasek_switch(0);
	}
	//okno na po³owie ekranu
	if(config_halfscreen==1){
		HDC screen = GetDC(GetDesktopWindow());
		int screen_w = GetDeviceCaps(screen, HORZRES);
		int screen_h = GetDeviceCaps(screen, VERTRES);
		DeleteDC(screen);
		SetWindowPos(hwnd,HWND_TOP,0,0,screen_w/2,screen_h-30,0);
    }
    //baza akordów na start (jeœli nie by³ otwierany wybrany plik)
    if(config_chordsbase_on_startup==1 && argc<2){
        chordsbase();
    }
	echo("wersja "+version);
}

void App::event_command(WPARAM wParam){
	button_click(wParam);
}

void App::event_dropfiles(char *filename){
	if(file_exists(filename)){
		open_file(filename);
		SetFocus(hctrl[2]);
	}else if(dir_exists(filename)){
		new_file();
		opened_file = filename;
		if(opened_file[opened_file.length()-1]!='\\') opened_file+="\\";
		SetWindowText(hctrl[0],opened_file.c_str());
		SetFocus(hctrl[0]);
		ss_clear(ss);
		ss<<"Nowy plik w folderze: \""<<filename<<"\"";
		echo(ss.str());
	}else{
		ss_clear(ss);
		ss<<"! B³¹d: nieprawid³owa œcie¿ka: \""<<filename<<"\"";
		echo(ss.str());
	}
	SetForegroundWindow(hwnd);
}

void App::event_resize(){
    int control_h = 22;
	RECT wnd_rect;
	GetClientRect(hwnd, &wnd_rect);
	window_w = wnd_rect.right-wnd_rect.left;
	window_h = wnd_rect.bottom-wnd_rect.top;
	if(pasek_shown){
        SetWindowPos(hctrl[2],HWND_TOP,0,control_h*4,window_w,window_h-control_h*4,0);
	}else{
        SetWindowPos(hctrl[2],HWND_TOP,0,control_h,window_w,window_h-control_h,0);
	}
    SetWindowPos(hctrl[1],HWND_TOP,0,0,window_w-20,control_h,SWP_NOMOVE);
    SetWindowPos(hctrl[0],HWND_TOP,0,0,window_w,control_h,SWP_NOMOVE);
	SetWindowPos(hctrl[22],HWND_TOP,window_w-20,0,0,0,SWP_NOSIZE);
    //resize panelu - 1. rz¹d
    SetWindowPos(hctrl[3],HWND_TOP,window_w*0/7,control_h*2,window_w/7,control_h,0);
    SetWindowPos(hctrl[16],HWND_TOP,window_w*1/7,control_h*2,window_w/7,control_h,0);
    SetWindowPos(hctrl[4],HWND_TOP,window_w*2/7,control_h*2,window_w/7,control_h,0);
    SetWindowPos(hctrl[13],HWND_TOP,window_w*3/7,control_h*2,window_w/7,control_h,0);
    SetWindowPos(hctrl[19],HWND_TOP,window_w*4/7,control_h*2,window_w/7,control_h,0);
    SetWindowPos(hctrl[20],HWND_TOP,window_w*5/7,control_h*2,window_w/7,control_h,0);
    SetWindowPos(hctrl[18],HWND_TOP,window_w*6/7,control_h*2,window_w/7,control_h,0);
	//2. rz¹d
    SetWindowPos(hctrl[9],HWND_TOP,window_w*0/7,control_h*3,window_w*2/7,control_h,0);
    SetWindowPos(hctrl[10],HWND_TOP,window_w*2/7,control_h*3,window_w*2/7,control_h,0);
    SetWindowPos(hctrl[11],HWND_TOP,window_w*4/7,control_h*3,window_w/7,control_h,0);
    SetWindowPos(hctrl[17],HWND_TOP,window_w*5/7,control_h*3,window_w/7,control_h,0);
    SetWindowPos(hctrl[5],HWND_TOP,window_w*6/7,control_h*3,window_w/7,control_h,0);
}

void App::event_screensave(){
	log("Screensaver stop");
	mouse_event(MOUSEEVENTF_MOVE,1,0,0,0);
	mouse_event(MOUSEEVENTF_MOVE,-1,0,0,0);
}

void App::event_timer(){
	autoscroll_exec();
}

void App::event_syskeydown(WPARAM wParam){
	if(wParam==VK_F10){
		wm_keydown(wParam);
	}
}

void App::event_appcommand(WPARAM wParam, LPARAM lParam){
    if(wParam==69){
        char newc = (char)lParam;
        if(newc==0){ //koniec przesy³ania nazwy pliku
            ss_clear(ss);
            ss<<"Otwieranie pliku z zewnêtrznego polecenia: "<<file_to_open;
            log(ss.str());
            open_file(file_to_open);
            file_to_open = "";
            SetForegroundWindow(hwnd);
            return;
        }
        file_to_open += newc;
        //string line = read_file_line("please_open");
        //system("del please_open");
        //set foreground
    }
}

void App::event_keydown(WPARAM wParam){
	if(wParam==VK_RETURN||wParam==VK_SPACE||wParam==VK_ESCAPE){
		SetFocus(hctrl[2]);
	}
	if(wParam==VK_F1){
		set_scroll(0);
	}else if(wParam==VK_F2){
		change_scroll(-35);
	}else if(wParam==VK_F3){
		change_scroll(+35);
	}else if(wParam==VK_F5){
		autoscroll_nowait(+autoscroll_interval*0.25);
	}else if(wParam==VK_F6){
		autoscroll_nowait(-autoscroll_interval*0.2);
	}else if(wParam==VK_F7){
		autoscroll_switch();
	}else if(wParam==VK_F8){
		if(autoscroll){
			autoscroll_off();
			echo("Autoscroll wy³¹czony");
		}else{
			autoscroll_nowait();
		}
	}else if(wParam==VK_F9){
		pasek_switch();
	}else if(wParam==VK_F11){
		fullscreen_set(!fullscreen_on);
	}
	if((GetAsyncKeyState(VK_CONTROL)&0x8000)&&!(GetAsyncKeyState(VK_MENU)&0x8000)){ //ctrl
		if(wParam=='A'){
			select_all();
		}else if(wParam=='S'){
			save_file();
		}else if(wParam=='R'){
			refresh_text();
		}else if(wParam=='F'){
			SetFocus(hctrl[9]);
		}else if(wParam==VK_ADD){ // +
			change_font_size(+1);
		}else if(wParam==VK_SUBTRACT){ // -
			change_font_size(-1);
		}else if(wParam==VK_OEM_3){ // `
			SetFocus(hctrl[0]);
			SendMessage(hctrl[0], EM_SETSEL, 0, -1);
		}else if(wParam==VK_LEFT){
			transpose(-1);
		}else if(wParam==VK_RIGHT){
			transpose(+1);
		}else if(wParam=='0'||wParam==VK_NUMPAD0){
			transpose(-transposed);
		}
	}
}
