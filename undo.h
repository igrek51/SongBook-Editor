#ifndef SYSTEM_H
#define SYSTEM_H

#include <iostream>
#include <vector>

using namespace std;

class UndoHistory {
public:
    UndoHistory(string *str);
    string content;
    unsigned int sel_start;
    unsigned int sel_end;
    int scroll;
};

class Undo {
public:
    vector<UndoHistory*> undos; //historia zmian: element 0 - najnowszy
    void revert();
    void save(string *str);
    void save();
    void reset();
};

#endif
