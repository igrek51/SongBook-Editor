#include "system.h"
#include "io.h"
#include <windows.h>
#include <commctrl.h>

System* System::instance = NULL;

System* System::geti(){
    if(instance == NULL){
        instance = new System();
    }
    return instance;
}

bool System::dir_exists(string name){
    DWORD ftyp = GetFileAttributesA(name.c_str());
    if(ftyp==INVALID_FILE_ATTRIBUTES) return false;
    if(ftyp&FILE_ATTRIBUTE_DIRECTORY) return true;
    return false;
}

void System::message_box(string title, string message){
    MessageBox(NULL, message.c_str(), title.c_str(), MB_OK|MB_ICONINFORMATION);
}

void System::get_args(){
    IO::geti()->get_args_from(GetCommandLine());
}

void System::set_text(string control_name, string text){
    SetWindowText(Controls::geti()->find(control_name), text.c_str());
	//UpdateWindow(hwnd);
}
