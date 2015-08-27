#ifndef IO_H
#define IO_H

#include <iostream>
#include <sstream>
#include <vector>

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
    int repeated_echo;
    void echo(string s);
    void echo(int e);
    //  Message Box
    void message_box(string title, string message);
    //  ARGUMENTY URUCHOMIENIA
    vector<string> args;
    void get_args_from(string args_text);
    bool is_arg(string parametr);
    void set_workdir();
};

string get_time();
void ss_clear(stringstream &sstream);

#endif
