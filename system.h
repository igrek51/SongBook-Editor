#ifndef SYSTEM_H
#define SYSTEM_H

#include <iostream>

using namespace std;

class System {
private:
	static System* instance;
public:
    static System* geti();

};

#endif
