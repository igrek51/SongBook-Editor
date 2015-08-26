#ifndef FILES_H
#define FILES_H

#include <iostream>
#include <vector>

using namespace std;

void clear_file(string filename);
bool file_exists(string name);
vector<string>* get_all_lines(string filename);

#endif
