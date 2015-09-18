#include "system.h"

bool is_control_pressed(){
    return (GetAsyncKeyState(VK_CONTROL)&0x8000) && !(GetAsyncKeyState(VK_MENU)&0x8000);
}

bool is_alt_pressed(){
    return ((GetAsyncKeyState(VK_MENU)&0x8000)&&!(GetAsyncKeyState(VK_CONTROL)&0x8000))||(GetAsyncKeyState(VK_RMENU)&0x8000);
}


bool EnumProcessWindowsProc(HWND hwnd, LPARAM lParam){
    WindowHWNDS *windowhwnds = reinterpret_cast<WindowHWNDS*>(lParam);
    char *str2 = new char[512];
    GetClassName(hwnd, str2, 512);
    string classname = str2;
	delete[] str2;
    if(windowhwnds->classname_cmp.length()==0 || classname==windowhwnds->classname_cmp){ //jeœli zgadza siê klasa (lub jest pusty wzorzec)
        windowhwnds->hwnds->push_back(hwnd); //dopisanie do listy
    }
    return true;
}

vector<HWND>* windows_list(string classname){
    WindowHWNDS* lista = new WindowHWNDS(classname);
    EnumWindows((WNDENUMPROC)EnumProcessWindowsProc, reinterpret_cast<LPARAM>(lista));
    vector<HWND>* hwnds = lista->hwnds;
    delete lista;
    return hwnds;
}

vector<HWND>* windows_diff(vector<HWND>* hwnds1, vector<HWND>* hwnds2){
    vector<HWND>* diff = new vector<HWND>;
    for(unsigned int i=0; i<hwnds2->size(); i++){ //przejœcie po elementach drugiej listy
        HWND szukaj = hwnds2->at(i);
        bool znalazlo = false;
        for(unsigned int j=0; j<hwnds1->size(); j++){ //szukanie w liœcie pierwszej
            if(hwnds1->at(j) == szukaj){
                znalazlo = true;
                break;
            }
        }
        if(!znalazlo){
            diff->push_back(szukaj); //dodanie do listy ró¿nic
        }
    }
    return diff;
}
