#ifndef STRINGS_H
#define STRINGS_H

#include <iostream>

using namespace std;

char string_char(string* str, int pos);
void string_char_set(string* str, int pos, char c);
void string_delete(string*& str, int &pos);
void string_insert(string*& str, int pos, char c);

string string_cutfromend(string s, int from_end = 1);

string trim_spaces(string s);
string trim_1crlf(string s);

char to_lowercase(char in);

#endif
