#include "app.h"
#include <richedit.h>

void App::autoscroll_exec(){
	IO::geti()->log("Timer...");
	if(Config::geti()->autoscroll_count==0){
		KillTimer(main_window, 1);
		SetTimer(main_window, 1, Config::geti()->autoscroll_interval, (TIMERPROC)NULL);
	}
	POINT mypoint;
	SendMessage(Controls::geti()->find("editor"), EM_GETSCROLLPOS, 0, (LPARAM)&mypoint);
	SCROLLINFO si;
	si.cbSize = sizeof(si);
	si.fMask = SIF_PAGE|SIF_RANGE|SIF_POS;
	GetScrollInfo(Controls::geti()->find("editor"), SB_VERT, &si);
	int scroll_max_pos = si.nMax - si.nPage + 1;
	mypoint.y += 2;
	if(mypoint.y>=scroll_max_pos || mypoint.y!=si.nPos+2){
		SendMessage(Controls::geti()->find("editor"), WM_VSCROLL, SB_BOTTOM, 0);
		autoscroll_off();
		IO::geti()->echo("Autoscroll zakoñczony - koniec pliku");
		return;
	}
	//last_scroll=GetScrollPos(hctrl[2],SB_VERT);
	SendMessage(Controls::geti()->find("editor"), EM_SETSCROLLPOS, 0, (LPARAM)&mypoint);
	//SendMessage(hctrl[2], EM_LINESCROLL, 0, 1);
	//SendMessage(hctrl[2], WM_VSCROLL, 1, 0);
	//int scroll2=GetScrollPos(hctrl[2],SB_VERT);
	Config::geti()->autoscroll_count++;
}

void App::autoscroll_on(){
	if(Config::geti()->autoscroll) autoscroll_off();
    Config::geti()->autoscroll_interval = Controls::geti()->get_int("autoscroll_interval");
    Config::geti()->autoscroll_wait = Controls::geti()->get_int("autoscroll_wait");
	if(Config::geti()->autoscroll_interval<0) return;
	if(Config::geti()->autoscroll_wait<0) return;
	Config::geti()->autoscroll = true;
	Config::geti()->autoscroll_count = 0;
    Controls::geti()->set_text("autoscroll", "Autoscroll: on");
    stringstream ss;
	ss<<"Autoscroll w³¹czony (opóŸnienie: "<<Config::geti()->autoscroll_wait<<" s, interwa³: "<<Config::geti()->autoscroll_interval<<" ms)";
	IO::geti()->echo(ss.str());
	SetTimer(main_window, 1, Config::geti()->autoscroll_wait*1000, (TIMERPROC)NULL);
	IO::geti()->log("Timer on");
}

void App::autoscroll_nowait(int change){
	if(Config::geti()->autoscroll) autoscroll_off();
    Config::geti()->autoscroll_interval = Controls::geti()->get_int("autoscroll_interval");
	if(Config::geti()->autoscroll_interval < 0) return;
	if(change != 0){
		Config::geti()->autoscroll_interval += change;
		if(Config::geti()->autoscroll_interval < 1) Config::geti()->autoscroll_interval = 1;
        Controls::geti()->set_text("autoscroll_interval", Config::geti()->autoscroll_interval);
	}
	Config::geti()->autoscroll = true;
	Config::geti()->autoscroll_count = 0;
	Controls::geti()->set_text("autoscroll", "Autoscroll: on");
	stringstream ss;
	ss<<"Autoscroll w³¹czony (interwa³: "<<Config::geti()->autoscroll_interval<<" ms)";
	IO::geti()->echo(ss.str());
	IO::geti()->log("Timer on - nowait");
	autoscroll_exec();
}

void App::autoscroll_off(){
	if(!Config::geti()->autoscroll) return;
	KillTimer(main_window, 1);
	Config::geti()->autoscroll = false;
    Controls::geti()->set_text("autoscroll", "Autoscroll: off");
	IO::geti()->log("Timer off");
}

void App::autoscroll_switch(){
	if(Config::geti()->autoscroll){
		autoscroll_off();
		IO::geti()->echo("Autoscroll wy³¹czony");
	}else{
		autoscroll_on();
	}
}
