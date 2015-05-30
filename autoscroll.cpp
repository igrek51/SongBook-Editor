#include "app.h"
#include <richedit.h>

void App::autoscroll_exec(){
	log("Timer...");
	if(autoscroll_count==0){
		KillTimer(hwnd,1);
		SetTimer(hwnd,1,autoscroll_interval,(TIMERPROC)NULL);
	}
	POINT mypoint;
	SendMessage(hctrl[2],EM_GETSCROLLPOS,0,(LPARAM)&mypoint);
	SCROLLINFO si;
	si.cbSize = sizeof(si);
	si.fMask = SIF_PAGE|SIF_RANGE|SIF_POS;
	GetScrollInfo(hctrl[2],SB_VERT,&si);
	int scroll_max_pos = si.nMax - si.nPage + 1;
	mypoint.y += 2;
	if(mypoint.y>=scroll_max_pos||mypoint.y!=si.nPos+2){
		SendMessage(hctrl[2],WM_VSCROLL,SB_BOTTOM,0);
		autoscroll_off();
		echo("Autoscroll zakoñczony - koniec pliku");
		return;
	}
	//last_scroll=GetScrollPos(hctrl[2],SB_VERT);
	SendMessage(hctrl[2],EM_SETSCROLLPOS,0,(LPARAM)&mypoint);
	//SendMessage(hctrl[2], EM_LINESCROLL, 0, 1);
	//SendMessage(hctrl[2], WM_VSCROLL, 1, 0);
	//int scroll2=GetScrollPos(hctrl[2],SB_VERT);
	autoscroll_count++;
}

void App::autoscroll_on(){
	if(autoscroll) autoscroll_off();
	char *str2 = new char[256];
	GetWindowText(hctrl[19],str2,256);
	autoscroll_interval = atoi(str2);
	GetWindowText(hctrl[20],str2,256);
	autoscroll_wait = atoi(str2);
	delete[] str2;
	if(autoscroll_interval<0) return;
	if(autoscroll_wait<0) return;
	autoscroll=true;
	autoscroll_count=0;
	SetWindowText(hctrl[18],"Autoscroll: on");
	ss_clear(ss);
	ss<<"Autoscroll w³¹czony (opóŸnienie: "<<autoscroll_wait<<" s, interwa³: "<<autoscroll_interval<<" ms)";
	echo(ss.str());
	SetTimer(hwnd,1,autoscroll_wait*1000,(TIMERPROC)NULL);
	log("Timer on");
}

void App::autoscroll_nowait(int change){
	if(autoscroll) autoscroll_off();
	char *str2 = new char[256];
	GetWindowText(hctrl[19],str2,256);
	autoscroll_interval = atoi(str2);
	delete[] str2;
	if(autoscroll_interval<0) return;
	if(change!=0){
		autoscroll_interval+=change;
		if(autoscroll_interval<1) autoscroll_interval=1;
		ss_clear(ss);
		ss<<autoscroll_interval;
		SetWindowText(hctrl[19],ss.str().c_str());
	}
	autoscroll=true;
	autoscroll_count=0;
	SetWindowText(hctrl[18],"Autoscroll: on");
	ss_clear(ss);
	ss<<"Autoscroll w³¹czony (interwa³: "<<autoscroll_interval<<" ms)";
	echo(ss.str());
	log("Timer on - nowait");
	autoscroll_exec();
}

void App::autoscroll_off(){
	if(!autoscroll) return;
	KillTimer(hwnd,1);
	autoscroll = false;
	SetWindowText(hctrl[18],"Autoscroll: off");
	log("Timer off");
}

void App::autoscroll_switch(){
	if(autoscroll){
		autoscroll_off();
		echo("Autoscroll wy³¹czony");
	}else{
		autoscroll_on();
	}
}
