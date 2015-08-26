#include "system.h"
#include <windows.h>

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
