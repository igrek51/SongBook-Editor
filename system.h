#ifndef SYSTEMOWE_H
#define SYSTEMOWE_H

#include <iostream>
#include <vector>
#define WINVER 0x0501 //WINXP
#include <windows.h>

using namespace std;

bool is_control_pressed();
bool is_alt_pressed();

class WindowHWNDS {
public:
    string classname_cmp;
    vector<HWND>* hwnds;
    WindowHWNDS(string classname_cmp){
        this->classname_cmp = classname_cmp;
        hwnds = new vector<HWND>;
    }
};

bool EnumProcessWindowsProc(HWND hwnd, LPARAM lParam);
vector<HWND>* windows_list(string classname);
vector<HWND>* windows_diff(vector<HWND>* hwnds1, vector<HWND>* hwnds2);

void set_workdir();
string get_app_path();

bool set_registry_default_value(HKEY hRootKey, string subkey, string value);
bool set_registry_value(HKEY hRootKey, string subkey, string variable, string value);

#endif
