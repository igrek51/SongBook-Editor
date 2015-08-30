#include "app.h"
#include <richedit.h>

void App::autoscroll_exec(){
	if(Config::geti()->autoscroll_count==0){
		KillTimer(main_window, 1);
		SetTimer(main_window, 1, Config::geti()->autoscroll_interval, (TIMERPROC)NULL);
	}
    if(!change_scroll(+Config::geti()->autoscroll_step)){
        autoscroll_off();
		IO::geti()->echo("Autoscroll zakoñczony - koniec pliku");
		return;
    }
	//last_scroll=GetScrollPos(hctrl[2],SB_VERT);
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
	IO::geti()->log("Timer autoscrolla w³¹czony");
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
	IO::geti()->log("Timer autoscrolla w³¹czony (bez opóŸnienia)");
	autoscroll_exec();
}

void App::autoscroll_off(){
	if(!Config::geti()->autoscroll) return;
	KillTimer(main_window, 1);
	Config::geti()->autoscroll = false;
    Controls::geti()->set_text("autoscroll", "Autoscroll: off");
	IO::geti()->log("Timer autoscrolla wy³¹czony");
}

void App::autoscroll_switch(){
	if(Config::geti()->autoscroll){
		autoscroll_off();
		IO::geti()->echo("Autoscroll wy³¹czony");
	}else{
		autoscroll_on();
	}
}
