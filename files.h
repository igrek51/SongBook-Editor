#ifndef FILES_H
#define FILES_H

#include <iostream>
#include <vector>

using namespace std;

void clear_file(string filename);
bool file_exists(string name);
bool dir_exists(string name);
vector<string>* get_all_lines(string filename);

char* open_file(string filename, int &file_size);
bool save_file(string filename, string content);

#endif
