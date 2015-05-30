#include "app.h"

int get_nr_chord(string chord){
	if(chord=="c") return 1;
	if(chord=="c#") return 2;
	if(chord=="d") return 3;
	if(chord=="d#") return 4;
	if(chord=="e") return 5;
	if(chord=="f") return 6;
	if(chord=="f#") return 7;
	if(chord=="g") return 8;
	if(chord=="g#") return 9;
	if(chord=="a") return 10;
	if(chord=="b") return 11;
	if(chord=="h") return 12;
	if(chord=="C") return 13;
	if(chord=="C#") return 14;
	if(chord=="D") return 15;
	if(chord=="D#") return 16;
	if(chord=="E") return 17;
	if(chord=="F") return 18;
	if(chord=="F#") return 19;
	if(chord=="G") return 20;
	if(chord=="G#") return 21;
	if(chord=="A") return 22;
	if(chord=="B") return 23;
	if(chord=="H") return 24;
	return 0;
}

string get_chord(int chord_nr){
	if(chord_nr==1) return "c";
	if(chord_nr==2) return "c#";
	if(chord_nr==3) return "d";
	if(chord_nr==4) return "d#";
	if(chord_nr==5) return "e";
	if(chord_nr==6) return "f";
	if(chord_nr==7) return "f#";
	if(chord_nr==8) return "g";
	if(chord_nr==9) return "g#";
	if(chord_nr==10) return "a";
	if(chord_nr==11) return "b";
	if(chord_nr==12) return "h";
	if(chord_nr==13) return "C";
	if(chord_nr==14) return "C#";
	if(chord_nr==15) return "D";
	if(chord_nr==16) return "D#";
	if(chord_nr==17) return "E";
	if(chord_nr==18) return "F";
	if(chord_nr==19) return "F#";
	if(chord_nr==20) return "G";
	if(chord_nr==21) return "G#";
	if(chord_nr==22) return "A";
	if(chord_nr==23) return "B";
	if(chord_nr==24) return "H";
	return "";
}

void App::transpose(int transponuj){
	if(transponuj==0) return;
	transposed += transponuj;
	char *str = load_text();
	string trans="";
	int nawias=0;
	char ch, ch_next;
	int chord_nr;
	for(unsigned int i=0; i<str_size; i++){
		ch=str[i];
		if(ch=='['){
			nawias=1;
			trans+=ch;
			continue;
		}else if(ch==']'){
			nawias=0;
			trans+=ch;
			continue;
		}
		if(i==str_size-1){ //ostatni znak
			trans+=ch;
			continue;
		}
		if(nawias==1){
			ch_next=str[i+1];
			//dwuznakowy
			ss_clear(ss);
			ss<<ch<<ch_next;
			chord_nr=get_nr_chord(ss.str());
			if(chord_nr!=0){
				if(chord_nr<=12){//mollowe
					chord_nr=(chord_nr-1+transponuj)%12+1;
					if(chord_nr<=0) chord_nr+=12;
				}else{ //durowe
					chord_nr=(chord_nr-13+transponuj)%12+13;
					if(chord_nr<=12) chord_nr+=12;
				}
				trans+=get_chord(chord_nr);
				i++;
				continue;
			}else{
				//jednoznakowy
				ss_clear(ss);
				ss<<ch;
				chord_nr=get_nr_chord(ss.str());
				if(chord_nr!=0){
					if(chord_nr<=12){//mollowe
						chord_nr=(chord_nr-1+transponuj)%12+1;
						if(chord_nr<=0) chord_nr+=12;
					}else{ //durowe
						chord_nr=(chord_nr-13+transponuj)%12+13;
						if(chord_nr<=12) chord_nr+=12;
					}
					trans+=get_chord(chord_nr);
					continue;
				}
			}
		}
		trans+=ch;
	}
	SetWindowText(hctrl[2],trans.c_str());
	refresh_text();
	delete[] str;
	ss_clear(ss);
	ss<<"Transpozycja akordów: ";
	if(transposed>0) ss<<"+";
	ss<<transposed;
	echo(ss.str());
}

string App::transpose_string(string in, int t){
	if(t%12==0) return in;
	string trans="";
	char ch, ch_next;
	int chord_nr;
	for(unsigned int i=0; i<in.length(); i++){
		ch=in[i];
		if(i==in.length()-1){ //ostatni znak
			ch_next=' ';
		}else{
			ch_next=in[i+1];
		}
		ch_next=in[i+1];
		//dwuznakowy
		ss_clear(ss);
		ss<<ch<<ch_next;
		chord_nr=get_nr_chord(ss.str());
		if(chord_nr!=0){
			if(chord_nr<=12){//mollowe
				chord_nr=(chord_nr-1+t)%12+1;
				if(chord_nr<=0) chord_nr+=12;
			}else{ //durowe
				chord_nr=(chord_nr-13+t)%12+13;
				if(chord_nr<=12) chord_nr+=12;
			}
			trans+=get_chord(chord_nr);
			i++;
			continue;
		}else{
			//jednoznakowy
			ss_clear(ss);
			ss<<ch;
			chord_nr=get_nr_chord(ss.str());
			if(chord_nr!=0){
				if(chord_nr<=12){//mollowe
					chord_nr=(chord_nr-1+t)%12+1;
					if(chord_nr<=0) chord_nr+=12;
				}else{ //durowe
					chord_nr=(chord_nr-13+t)%12+13;
					if(chord_nr<=12) chord_nr+=12;
				}
				trans+=get_chord(chord_nr);
				continue;
			}
		}
		trans+=ch;
	}
	return trans;
}
