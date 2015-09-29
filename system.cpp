#include "system.h"
#include "io.h"
#include "strings.h"

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


void set_workdir(){
    vector<string> args = IO::geti()->args;
    if(args.size()==0) return;
    string arg0 = trim_quotes(args.at(0));
    int last_slash = -1;
	for(int i=(int)arg0.length()-1; i>=0; i--){ //szukaj od koñca
        if(arg0[i]=='\\' || arg0[i]=='/'){
            last_slash = i;
            break;
        }
	}
	if(last_slash==-1) return; //nie by³o slasha - katalog jest ju¿ ustawiony
	string workdir = arg0.substr(0, last_slash);
    if(SetCurrentDirectory(workdir.c_str())==0){
        IO::geti()->message_box("B³¹d", "B³¹d zmiany katalogu roboczego na: "+workdir);
    }
    IO::geti()->log("Katalog roboczy: "+workdir);
}

string get_app_path(){
    //workdir
    char *str = new char [512];
    if(GetCurrentDirectory(512, str)==0){
        delete[] str;
        IO::geti()->error("B³¹d odczytywania katalogu roboczego.");
        return "";
    }
    string workdir = str;
    delete[] str;
    //exe name
    vector<string> args = IO::geti()->args;
    if(args.size()==0) return "";
    string arg0 = trim_quotes(args.at(0));
    int last_slash = -1;
	for(int i=(int)arg0.length()-1; i>=0; i--){ //szukaj od koñca
        if(arg0[i]=='\\' || arg0[i]=='/'){
            last_slash = i;
            break;
        }
	}
	string exe_name = arg0.substr(last_slash+1);
    return workdir+"\\"+exe_name;
}


bool set_registry_default_value(HKEY hRootKey, string subkey, string value){
    IO::geti()->log("Zapisywanie domyœlnej wartoœci klucza rejestru: "+subkey+" = "+value);
    if(RegSetValue(hRootKey, subkey.c_str(), REG_SZ, value.c_str(), value.length()) != ERROR_SUCCESS){
        IO::geti()->error("B³¹d zapisywania do rejestru.");
        return false;
    }
    return true;
}

bool set_registry_value(HKEY hRootKey, string subkey, string variable, string value){
    IO::geti()->log("Zapisywanie wartoœci klucza rejestru: "+subkey+"\\"+variable+" = "+value);
    HKEY hKey;
    if(RegOpenKeyEx(hRootKey, subkey.c_str(), 0, KEY_READ|KEY_SET_VALUE, &hKey) != ERROR_SUCCESS){
        IO::geti()->error("B³¹d otwierania klucza rejestru - brak uprawnieñ.");
        return false;
    }
    RegSetValueEx(hKey, variable.c_str(), 0, REG_SZ, (const BYTE*)value.c_str(), value.length());
    if(RegSetValue(hRootKey, subkey.c_str(), REG_SZ, value.c_str(), value.length()) != ERROR_SUCCESS){
        IO::geti()->error("B³¹d zapisywania do rejestru.");
        RegCloseKey(hKey);
        return false;
    }
    RegCloseKey(hKey);
    return true;
}
