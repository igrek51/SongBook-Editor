#include "app.h"
#include <commctrl.h>
#include <richedit.h>

void App::wm_create(HWND *window){
	hwnd = *window;
	//parametry
	get_argv();
    //jeœli aplikacja jest ju¿ uruchomiona
    if(argc==2&&window2!=NULL){ //jeden dodatkowy parametr - nazwa pliku do otwarcia
        //save_file_line("please_open",argv[1]);
        for(unsigned int i=0; i<argv[1].length(); i++){
            SendMessage(window2, 0x0319, 69, (char)argv[1][i]);
        }
        SendMessage(window2, 0x0319, 69, 0);
        DestroyWindow(hwnd);
        return;
    }
	//katalog roboczy
	set_workdir();
	//ustawienia
	load_config();
	transposed=0;
	autoscroll=false;
	opened_file="";
	for(int i=0; i<9; i++) tekst_w[i]="";
	repeated_echo = 0;
	last_echo = "";
    file_to_open = "";
	//log
	if(config_log_enabled==1) clear_file("log.txt");
	log("Hello World...");
	//kontrolki
    int x_paint, y_paint, buttonw = window_w/7;
    int control_h = 22;
    x_paint=0; y_paint=40;
    hctrl[1]=create_static("",0,0,window_w-20,control_h,SS_CENTER|SS_CENTERIMAGE); //output
    hctrl[0]=create_edit("",0,control_h,window_w,control_h,0); //wiersz poleceñ

    hctrl[3]=create_button("Nowy",x_paint,y_paint,buttonw,20,1); x_paint+=buttonw;
    hctrl[16]=create_button("Wczytaj",x_paint,y_paint,buttonw,20,12); x_paint+=buttonw;
    hctrl[4]=create_button("Zapisz",x_paint,y_paint,buttonw,20,2); x_paint+=buttonw;
    hctrl[13]=create_button("Baza akordów",x_paint,y_paint,buttonw,20,9); x_paint+=buttonw;

    hctrl[19]=create_edit("",x_paint,y_paint,buttonw,20,ES_CENTER); x_paint+=buttonw;//autoscroll_interval
    hctrl[20]=create_edit("",x_paint,y_paint,buttonw,20,ES_CENTER); x_paint+=buttonw;//autoscroll_wait
    hctrl[18]=create_button("Autoscroll: off",x_paint,y_paint,buttonw,20,14); x_paint+=buttonw;

    x_paint = 0; y_paint+=20;
    hctrl[9]=create_edit("",x_paint,y_paint,buttonw*2,20,0); x_paint+=buttonw*2;//szukany ci¹g
    hctrl[10]=create_edit("",x_paint,y_paint,buttonw*2,20,0); x_paint+=buttonw*2;
    hctrl[11]=create_button("Zamieñ",x_paint,y_paint,buttonw,20,7); x_paint+=buttonw;
    hctrl[17]=create_button("ZnajdŸ",x_paint,y_paint,buttonw,20,13); x_paint+=buttonw;
    hctrl[5]=create_button("Analizuj",x_paint,y_paint,buttonw,20,3); x_paint+=buttonw;

    hctrl[22]=create_button("^",window_w-control_h,0,20,control_h,16); //schowanie paska
	//edytor
	if(LoadLibrary("RICHED32.DLL")==NULL){
		echo("B³¹d: brak biblioteki RICHED32.DLL");
		return;
	}
    hctrl[2]=CreateWindowEx(WS_EX_CLIENTEDGE,RICHEDIT_CLASS,"",WS_CHILD|WS_VISIBLE|WS_VSCROLL|ES_MULTILINE|ES_DISABLENOSCROLL,0,80,window_w,window_h-80,hwnd,(HMENU)100,*hInst,0);
	//autoscroll edits
	ss_clear(ss);
	ss<<autoscroll_interval;
	SetWindowText(hctrl[19],ss.str().c_str());
	ss_clear(ss);
	ss<<autoscroll_wait;
	SetWindowText(hctrl[20],ss.str().c_str());
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

void App::wm_command(WPARAM wParam){
	button_click(wParam);
}

void App::wm_dropfiles(char *filename){
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

void App::wm_resize(){
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

void App::wm_screensave(){
	log("Screensaver stop");
	mouse_event(MOUSEEVENTF_MOVE,1,0,0,0);
	mouse_event(MOUSEEVENTF_MOVE,-1,0,0,0);
}

void App::wm_timer(){
	autoscroll_exec();
}

void App::wm_syskeydown(WPARAM wParam){
	if(wParam==VK_F10){
		wm_keydown(wParam);
	}
}

void App::wm_appcommand(WPARAM wParam, LPARAM lParam){
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

void App::wm_keydown(WPARAM wParam){
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
