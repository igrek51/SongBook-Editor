#include "app.h"

App* App::instance = NULL;

App* App::geti(){
    if(instance == NULL){
        instance = new App();
    }
    return instance;
}


App::App(int w, int h, string version, int ctrls){
    this->version = version+" (win)";
	window_w=w;
	window_h=h;
	ctrls_num = ctrls;
	if(ctrls>0){
		hctrl = new HWND [ctrls_num];
		wndproc_old = new WNDPROC [ctrls_num];
		for(int i=0; i<ctrls_num; i++){
			hctrl[i]=NULL;
			wndproc_old[i]=NULL;
		}
	}else{
		hctrl = NULL;
		wndproc_old = NULL;
	}
    window2 = NULL;
}

App::~App(){
	Log::log("Clean up...");
	autoscroll_off();
	for(int i=0; i<ctrls_num; i++){
		un_subclass(i);
		if(hctrl[i]!=NULL) DestroyWindow(hctrl[i]);
    }
	Log::log("Goodbye World...");
	PostQuitMessage(0);
}

void App::check_instance(char *classname){
    window2 = FindWindow(classname,NULL);
}
