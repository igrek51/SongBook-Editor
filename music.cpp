#include "music.h"

int get_chord_number(string chord){
    for(int i=0; i<24; i++){
        if(chord == sound_names[i]) return i+1;
    }
	return 0;
}

string get_chord_name(int chord_nr){
    if(chord_nr <= 0) return "";
    if(chord_nr > 24) return "";
    return sound_names[chord_nr-1];
}

string transpose_chords(string in, int t){
    //transpozycja dla samych akord�w
	if(t%12 == 0) return in;
	string trans = "";
    string temp_chord;
	int chord_nr;
	for(unsigned int i=0; i<in.length(); i++){
		if(i==in.length()-1){ //ostatni znak
            temp_chord = in.substr(i, 1);
		}else{
            temp_chord = in.substr(i, 2);
		}
		//pr�ba rozpoznania dwuznakowego akordu
		chord_nr = get_chord_number(temp_chord);
		if(chord_nr != 0){
			if(chord_nr <= 12){ //mollowe: 1 - 12
				chord_nr = (chord_nr-1+t)%12 + 1; //transpozycja
				if(chord_nr <= 0) chord_nr += 12;
			}else{ //durowe: 13 - 24
				chord_nr = (chord_nr-13+t)%12 + 13; //transpozycja
				if(chord_nr <= 12) chord_nr += 12;
			}
			trans += get_chord_name(chord_nr);
			i++; //omini�cie jednego znaku
		}else{
			//pr�ba rozpoznania jednoznakowego akordu
            temp_chord = temp_chord.substr(0, 1);
			chord_nr = get_chord_number(temp_chord);
			if(chord_nr != 0){
				if(chord_nr <= 12){ //mollowe: 1 - 12
					chord_nr = (chord_nr-1+t)%12 + 1;
					if(chord_nr <= 0) chord_nr += 12;
				}else{ //durowe: 13 - 24
					chord_nr=(chord_nr-13+t)%12 + 13;
					if(chord_nr <= 12) chord_nr += 12;
				}
				trans += get_chord_name(chord_nr);
			}else{
                //nie rozpoznano �adnego akordu - przepisanie znaku bez zmian
                trans += in[i];
            }
		}
	}
	return trans;
}

string transpose_string(string in, int t){
    //transpozycja dla wszystkich znak�w (z rozpoznawaniem sekcji akord�w)
    if(t%12 == 0) return in;
	string trans = "";
    string chords_section;
    int nawias_last = -1; // brak nawiasu otwieraj�cego
    char ch;
	for(int i=0; i<(int)in.length(); i++){
		ch = in[i];
		if(ch == '['){
            nawias_last = i+1; //zapami�taj pocz�tek sekcji
			trans += ch; //przepisz znak
		}else if(ch == ']'){
            if(nawias_last == -1){ //je�li nie by�o nawiasu otwieraj�cego
                trans += ch; //przepisz znak nawiasu
                continue;
            }
            chords_section = in.substr(nawias_last, i-nawias_last); //wytnij sekcj� akord�w
            trans += transpose_chords(chords_section, t) + ch; //dopisz transponowane akordy (plus znak nawiasu)
            nawias_last = -1; //wyzeruj nawias
		}else if(nawias_last == -1){ //je�li nie by�o nawiasu otwieraj�cego
            trans += ch; //przepisz znak
        }else if(i == (int)in.length()-1){ //je�li by� nawias otwieraj�cy i jest to ostatni znak
            trans += in.substr(nawias_last); //przepisz znaki od pocz�tku sekcji do ko�ca
		}
	}
    return trans;
}
