#include "app.h"
#include "version.h"

App* App::instance = NULL;

App* App::geti(){
    if(instance == NULL){
        instance = new App();
    }
    return instance;
}


App::App(){
    version = VERSION+" (win)";
    //  TODO
    window2 = NULL;
}

App::~App(){
	IO::geti()->log("Clean up...");
	autoscroll_off();
    delete Controls::geti();
	IO::geti()->log("Goodbye World...");
	PostQuitMessage(0);
}

void App::check_instance(string classname){
    //  TODO
    window2 = FindWindow(classname.c_str(),NULL);
}
