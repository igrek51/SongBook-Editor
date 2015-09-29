#ifndef IO_H
#define IO_H

#include <iostream>
#include <sstream>
#include <vector>
#include <ctime>

using namespace std;

class IO {
private:
	static IO* instance;
    IO();
public:
    static IO* geti();
    static IO* i();
    //  LOG
    bool log_init;
    vector<string> log_buffer;
    void clear_log();
    void delete_log();
    void log(string l);
    void log(int l);
    void log(string s, int l);
    void error(string l, bool show_output = true);
    void critical_error(string l);
    //  ECHO
    string last_echo;
    int repeated_echo;
    void echo(string s);
    void echo(int e);
    //  Message Box
    void message_box(string title, string message = "");
    int message_box_yesnocancel(string title, string message);
    int message_box_yesno(string title, string message);
    //  ARGUMENTY URUCHOMIENIA
    vector<string> args;
    void get_args();
    void get_args_from(string args_text);
    bool is_arg(string parametr);
    //  Pomiar czasu wykonania
    clock_t clock_last;
    void clock_checkpoint(string comment);
};

#endif
