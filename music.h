#ifndef TRANSPOSE_H
#define TRANSPOSE_H

#include <iostream>

using namespace std;

const string sound_names[] = {
    "c", "c#", "d", "d#", "e", "f", "f#", "g", "g#", "a", "b", "h",
    "C", "C#", "D", "D#", "E", "F", "F#", "G", "G#", "A", "B", "H"
};

int get_chord_number(string chord);
string get_chord_name(int chord_nr);

string transpose_chords(string in, int t);
string transpose_string(string in, int t);

#endif
