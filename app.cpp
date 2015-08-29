#include "app.h"
#include "version.h"

App* App::instance = NULL;

App* App::geti(){
    if(instance == NULL){
        instance = new App();
    }
    return instance;
}

App* App::i(){
    return geti();
}

App::App(){
    version = VERSION;
    version += " (win)";
    instancja2 = NULL;
}

App::~App(){
	IO::geti()->log("Sprz¹tanie...");
	autoscroll_off();
    delete Controls::geti();
	IO::geti()->log("Goodbye World...");
	PostQuitMessage(0);
}

void App::check_instance(string classname){
    instancja2 = FindWindow(classname.c_str(),NULL);
}
