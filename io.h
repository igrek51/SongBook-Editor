#ifndef IO_H
#define IO_H

#include <iostream>

using namespace std;

class IO {
private:
	static IO* instance;
public:
    static IO* geti();
	IO();
    //  LOG
    void clear_log();
    void log(string l);
    void log(int l);
    void error(string l);
    void critical_error(string l);
    //  ECHO
    string last_echo;
    void echo(string s);
    void echo(int e);
    //  ARGUMENTY URUCHOMIENIA


};

string get_time();
void ss_clear(stringstream &sstream);

#endif
