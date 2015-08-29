#include "app.h"
#include <richedit.h>

bool App::skanuj(){
	bool changed = false;
	string* str = load_text();
	int nawias = 0;
	//analiza pliku
	for(int i=0; (unsigned int)i<str->length(); i++){
		//dwa puste nawiasy []
		if(string_char(str, i)=='[' && string_char(str, i+1)==']'){
			del_char(str,str_size,i);
			i++;
			del_char(str,str_size,i);
			log("Analiza: puste nawiasy []");
			changed=true; continue;
		}
		//nadmierny nawias [ (zamiast ])
		if(string_char(str, i)=='['){
			if(nawias==0){
				nawias=1;
			}else{
				str[i]=']';
				nawias=0;
				log("Analiza: nieprawid³owy nawias zamykaj¹cy: [");
				changed=true; continue;
			}
		}
		//nadmierny nawias ]
		if(string_char(str, i)==']'){
			if(nawias==1){
				nawias=0;
			}else{
				str[i]='[';
				nawias=1;
				log("Analiza: nieprawid³owy nawias otwieraj¹cy: ]");
				changed=true; continue;
			}
		}
		if(string_char(str, i)==' '){
			//spacje na pocz¹tku wiersza
			if(i==0||get_str_c(str,str_size,i-1)=='\n'){
				del_char(str,str_size,i);
				log("Analiza: spacje na pocz¹tku wiersza");
				changed=true; continue;
			}
			//spacje na koñcu wiersza
			if(i==(int)str_size-1||(get_str_c(str,str_size,i+1)=='\r'&&get_str_c(str,str_size,i+2)=='\n')){
				del_char(str,str_size,i);
				log("Analiza: spacje na koñcu wiersza");
				changed=true; continue;
			}
		}
		//ostatni enter
		if(i==(int)str_size-2){
			if(string_char(str, i)=='\r' && string_char(str, i+1)=='\n'){
				del_char(str,str_size,i);
				log("Analiza: ostatni enter");
				changed=true; continue;
			}
		}
		if(i==(int)str_size-1&&string_char(str, i)=='\n'){
			del_char(str,str_size,i);
			log("Analiza: ostatni enter LF");
			changed=true; continue;
		}
		//pierwszy enter
		if(i==0&&(string_char(str, i)=='\r' || string_char(str, i)=='\n')){
			del_char(str,str_size,i);
			log("Analiza: pierwszy enter");
			changed=true; continue;
		}
		//podwójne spacje
		if(nawias==0 && string_char(str, i)==' ' && string_char(str, i+1)==' '){
			del_char(str,str_size,i);
			log("Analiza: podwójne spacje");
			changed=true; continue;
		}
		//po nawiasie [ nie ma spacji
		if(string_char(str, i-1)=='[' && string_char(str, i)==' '){
			del_char(str,str_size,i);
			log("Analiza: spacja po nawiasie [");
			changed=true; continue;
		}
		//przed nawiasem ] nie ma spacji
		if(string_char(str, i)==' ' && string_char(str, i+1)==']'){
			del_char(str,str_size,i);
			log("Analiza: spacja przed nawiasem ]");
			changed=true; continue;
		}
		//spacja lub enter przed nawiasem [
		if(string_char(str, i)=='[' && i>0){
			if(string_char(str, i-1)!='\n' && string_char(str, i-1)!=' '){
				insert_char(str, str_size, i, ' ');
				i++;
				log("Analiza: brak spacji lub entera przed nawiasem");
				changed=true; continue;
			}
		}
		//tab na spacjê
		if(string_char(str, i)=='\t'){
			str[i]=' ';
			log("Analiza: tabulator zamiast spacji");
			changed=true; continue;
		}
		//fis -> f#
		if(nawias==1){
			if(string_char(str, i)=='i' && string_char(str, i+1)=='s'){
				str[i+1]='#';
				del_char(str,str_size,i);
				log("Analiza: konwersja \"is\" na \"#\"");
				changed=true; continue;
			}
		}
		//samo \n (bez \r)
		if(i>0){
			if(string_char(str, i)=='\n' && string_char(str, i-1)!='\r'){
				insert_char(str, str_size, i, '\r');
				i++;
				changed=true; continue;
			}
		}
		//samo \r (bez \n)
		if(i>0){
			if(string_char(str, i-1)=='\r' && string_char(str, i)!='\n'){
				insert_char(str, str_size, i, '\n');
				i++;
				changed=true; continue;
			}
		}
	}
	if(nawias!=0){
		insert_char(str, str_size, str_size, ']');
		changed=true;
		log("Analiza: brak nawiasu zamykaj¹cego ] na koñcu");
	}
	save_text(str);
	return changed;
}

void App::usun_akordy(){
    /*
	char *str = load_text();
	unsigned int sel_start, sel_end;
	if(get_selected(sel_start,sel_end,str)==false){
		sel_start=0;
		sel_end=str_size;
	}
	int nawias=0;
	for(int i=sel_start; i<(int)sel_end;i++){
		if(get_str_c(str,str_size,i)=='['){
			nawias=1;
			del_char(str,str_size,i);
			sel_end--;
			continue;
		}
		if(get_str_c(str,str_size,i)==']'){
			nawias=0;
			del_char(str,str_size,i);
			sel_end--;
			continue;
		}
		if(nawias==1){
			del_char(str,str_size,i);
			sel_end--;
			continue;
		}
	}
	save_text(str);
	echo("Usuniêto akordy");
    */
}

void App::usun_wersje(){
    /*
	char *str = load_text();
	int nawias=0;
	for(int i=0; (unsigned int)i<str_size; i++){
		if(get_str_c(str,str_size,i)=='['){
			nawias=1;
			continue;
		}
		if(get_str_c(str,str_size,i)==']'){
			nawias=0;
			continue;
		}
		if(get_str_c(str,str_size,i+1)=='|'&&nawias>0&&get_str_c(str,str_size,i)==' '){
			del_char(str,str_size,i);
			continue;
		}
		if(get_str_c(str,str_size,i)=='|'&&nawias>0){
			nawias++;
			del_char(str,str_size,i);
			continue;
		}
		if(nawias>1){
			del_char(str,str_size,i);
			continue;
		}
	}
	save_text(str);
	echo("Usuniêto alternatywne wersje akordów");
    */
}

void App::zamien(){
    /*
	char *str1 = new char[256];
	GetWindowText(hctrl[9],str1,256);
	if(strlen(str1)==0){
		echo("! Szukany ciag jest pusty");
		delete[] str1;
		return;
	}
	char *str2= new char[256];
	GetWindowText(hctrl[10],str2,256);
	char *str = load_text();
	string nowy;
	int licznik=0;
	unsigned int sel_start=0, sel_end=str_size;
	if(get_selected(sel_start,sel_end,str)==true){
		sel_end+=-strlen(str1)+1;
	}else{
		//szukanie w ca³ym zakresie
		sel_start = 0;
		sel_end = str_size;
	}
	for(unsigned int i=0; i<str_size; i++){
		//pocz¹tek i koniec
		if(i<sel_start||i>=sel_end){
			nowy+=str[i];
			continue;
		}
		//szukanie wzorca
		if(i<=str_size-strlen(str1)){
			bool found=true;
			for(unsigned int j=i; j<i+strlen(str1); j++){
				if(str[j]!=str1[j-i]){
					found=false;
					break;
				}
			}
			if(found){
				nowy+=str2;
				i+=strlen(str1)-1;
				licznik++;
				continue;
			}
		}
		nowy+=str[i];
	}
	delete[] str;
	delete[] str1;
	delete[] str2;
	SetWindowText(hctrl[2],nowy.c_str());
	refresh_text();
	if(licznik==0){
		echo("Nie znaleziono wyst¹pieñ.");
	}else{
		ss_clear(ss);
		ss<<"Zamieniono "<<licznik;
		if(licznik==1) ss<<" wyst¹pienie";
		else if(licznik/10!=1&&(licznik%10==2||licznik%10==3||licznik%10==4)) ss<<" wyst¹pienia";
		else ss<<" wyst¹pieñ";
		ss<<" tekstu.";
		echo(ss.str());
	}
    */
}

char to_lowercase(char in){
	if(in>='A'&&in<='Z') return in-'A'+'a';
	if(in=='¥') return '¹';
	if(in=='¯') return '¿';
	if(in=='Œ') return 'œ';
	if(in=='') return 'Ÿ';
	if(in=='Ê') return 'ê';
	if(in=='Æ') return 'æ';
	if(in=='Ñ') return 'ñ';
	if(in=='Ó') return 'ó';
	if(in=='£') return '³';
	return in;
}

bool App::znajdz_w(char *str, unsigned int start, unsigned int end, char *wzor){
    /*
	for(unsigned int i=start; i<=end-strlen(wzor); i++){
		//szukanie wzorca
		bool found=true;
		for(unsigned int j=0; j<strlen(wzor); j++){
			if(to_lowercase(str[j+i])!=to_lowercase(wzor[j])){
				found=false;
				break;
			}
		}
		if(found){
			echo("Znaleziono fragment");
			set_selected(i,i+strlen(wzor),str);
			SetFocus(hctrl[9]);
			delete[] str;
			delete[] wzor;
			SendMessage(hctrl[2], EM_SCROLLCARET, 0, 0);
			return true;
		}
	}
    */
	return false;
}

void App::znajdz(){
    /*
	char *wzor = new char[256];
	GetWindowText(hctrl[9],wzor,256);
	if(strlen(wzor)==0){
		echo("! Szukany ciag jest pusty");
		delete[] wzor;
		return;
	}
	char *str = load_text();
	get_selected(last_sel_start,last_sel_end,str);
	if(znajdz_w(str,last_sel_start+1,str_size,wzor)) return;
	if(last_sel_start+1+strlen(wzor)>str_size) last_sel_start=str_size-1-strlen(wzor);
	if(znajdz_w(str,0,last_sel_start+1+strlen(wzor),wzor)) return;
	delete[] str;
	delete[] wzor;
	echo("Nie znaleziono fragmentu");
    */
}

void App::wstaw_tekst(int nrt){
    /*
	if(nrt<1||nrt>9) return;
	string add_text = tekst_w[nrt-1];
	if(add_text.length()==0){
		ss_clear(ss);
		ss<<"! Tekst nr "<<nrt<<" jest pusty";
		echo(ss.str());
		return;
	}
	SendMessage(hctrl[2], EM_REPLACESEL, 0, (LPARAM)(add_text.c_str()));
	ss_clear(ss);
	ss<<"Wstawiono tekst nr "<<nrt<<": "<<add_text;
	echo(ss.str());
    */
}

void App::zapisz_tekst(int nrt){
    /*
	if(nrt<1||nrt>9) return;
	str_size = GetWindowTextLength(hctrl[2]);
	char *selected = new char [str_size];
	SendMessage(hctrl[2],EM_GETSELTEXT,0,(LPARAM)selected);
	if(strlen(selected)==0){
		echo("! Nie zaznaczono tekstu do zapisania");
		delete[] selected;
		return;
	}
	tekst_w[nrt-1] = selected;
	ss_clear(ss);
	ss<<"Zapisano tekst nr "<<nrt<<": "<<selected;
	delete[] selected;
	echo(ss.str());
    */
}

void App::dodaj_nawias(){
    /*
	char *str = load_text();
	unsigned int sel_start, sel_end;
	get_selected(sel_start,sel_end,str);
	if(sel_start==sel_end){
		echo("! Nie zaznaczono ¿adnego fragmentu");
		delete[] str;
		return;
	}
	insert_char(str,str_size,sel_end,']');
	insert_char(str,str_size,sel_start,'[');
	last_sel_end+=2;
	save_text(str);
    */
}

void App::dodaj_alternatywne(){
    /*
	if(transposed==0){
		echo("! Zerowe przesuniêcie akordów");
		return;
	}
	char *str = load_text();
	string nowy="", akord="";
	char ch;
	int nawias=0;
	for(unsigned int i=0; i<str_size; i++){
		ch=str[i];
		if(ch=='['){
			nawias=1;
			nowy+=ch;
			continue;
		}
		if(ch==']'){
			if(akord.length()>0){
				 nowy+=" | ";
				 nowy+=transpose_string(akord,transposed);
			}
			akord="";
			nawias=0;
			nowy+=ch;
			continue;
		}
		if(ch=='|'&&nawias>=1){
			nawias++;
			nowy+=ch;
			continue;
		}
		if(i<str_size-1&&nawias>=1){
			if(ch==' '&&str[i+1]=='|'){
				nowy+=ch;
				continue;
			}
		}
		if(nawias==1){
			akord+=ch;
		}
		nowy+=ch;
	}
	SetWindowText(hctrl[2],nowy.c_str());
	refresh_text();
	delete[] str;
	echo("Dodano alternatywn¹ wersjê akordów");
    */
}
