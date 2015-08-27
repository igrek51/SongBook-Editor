#ifndef CONTROLS_H
#define CONTROLS_H

#include <iostream>
#include <vector>

using namespace std;

class Controls {
private:
	static Controls* instance;
public:
    static Controls* geti();
	Controls();
    vector<Control*> controls;
    HWND find(string name);
};

class Control {
public:
    Control(HWND handle, string name);
    HWND handle;
    string name;
}

#endif
