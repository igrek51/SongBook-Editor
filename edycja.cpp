#include "app.h"
#include "strings.h"
#include <richedit.h>

bool App::skanuj_1(){
	bool changed = false;
	string* str = load_text();
	int nawias = 0;
	//analiza pliku
	for(int i=0; (unsigned int)i<str->length(); i++){
		//dwa puste nawiasy []
		if(string_char(str, i)=='[' && string_char(str, i+1)==']'){
			string_delete(str, i);
			i++;
			string_delete(str, i);
			IO::geti()->log("Analiza: puste nawiasy []");
			changed=true; continue;
		}
		//nadmierny nawias [ (zamiast ])
		if(string_char(str, i)=='['){
			if(nawias==0){
				nawias=1;
			}else{
				string_char_set(str, i, ']');
				nawias=0;
				IO::geti()->log("Analiza: nieprawid³owy nawias zamykaj¹cy: [");
				changed=true; continue;
			}
		}
		//nadmierny nawias ]
		if(string_char(str, i)==']'){
			if(nawias==1){
				nawias=0;
			}else{
				string_char_set(str, i, '[');
				nawias=1;
				IO::geti()->log("Analiza: nieprawid³owy nawias otwieraj¹cy: ]");
				changed=true; continue;
			}
		}
		if(string_char(str, i)==' '){
			//spacje na pocz¹tku wiersza
			if(i==0 || string_char(str, i-1)=='\n'){
				string_delete(str, i);
				IO::geti()->log("Analiza: spacje na pocz¹tku wiersza");
				changed=true; continue;
			}
			//spacje na koñcu wiersza
			if(i==(int)str->length()-1 || (string_char(str, i+1)=='\r' && string_char(str, i+2)=='\n')){
				string_delete(str, i);
				IO::geti()->log("Analiza: spacje na koñcu wiersza");
				changed=true; continue;
			}
		}
		//ostatni enter
		if(i==(int)str->length()-2){
			if(string_char(str, i)=='\r' && string_char(str, i+1)=='\n'){
				string_delete(str, i);
				IO::geti()->log("Analiza: ostatni enter");
				changed=true; continue;
			}
		}
		if(i==(int)str->length()-1 && string_char(str, i)=='\n'){
			string_delete(str, i);
			IO::geti()->log("Analiza: ostatni enter LF");
			changed=true; continue;
		}
		//pierwszy enter
		if(i==0&&(string_char(str, i)=='\r' || string_char(str, i)=='\n')){
			string_delete(str, i);
			IO::geti()->log("Analiza: pierwszy enter");
			changed=true; continue;
		}
		//podwójne spacje
		if(nawias==0 && string_char(str, i)==' ' && string_char(str, i+1)==' '){
			string_delete(str, i);
			IO::geti()->log("Analiza: podwójne spacje");
			changed=true; continue;
		}
		//po nawiasie [ nie ma spacji
		if(string_char(str, i-1)=='[' && string_char(str, i)==' '){
			string_delete(str, i);
			IO::geti()->log("Analiza: spacja po nawiasie [");
			changed=true; continue;
		}
		//przed nawiasem ] nie ma spacji
		if(string_char(str, i)==' ' && string_char(str, i+1)==']'){
			string_delete(str, i);
			IO::geti()->log("Analiza: spacja przed nawiasem ]");
			changed=true; continue;
		}
		//spacja lub enter przed nawiasem [
		if(string_char(str, i)=='[' && i>0){
			if(string_char(str, i-1)!='\n' && string_char(str, i-1)!=' '){
				string_insert(str, i, ' ');
				i++;
				IO::geti()->log("Analiza: brak spacji lub entera przed nawiasem");
				changed=true; continue;
			}
		}
		//tab na spacjê
		if(string_char(str, i)=='\t'){
			string_char_set(str, i, ' ');
			IO::geti()->log("Analiza: tabulator zamiast spacji");
			changed=true; continue;
		}
		//fis -> f#
		if(nawias==1){
			if(string_char(str, i)=='i' && string_char(str, i+1)=='s'){
				string_char_set(str, i+1, '#');
				string_delete(str, i);
				IO::geti()->log("Analiza: konwersja \"is\" na \"#\"");
				changed=true; continue;
			}
		}
		//samo \n (bez \r)
		if(i>0){
			if(string_char(str, i)=='\n' && string_char(str, i-1)!='\r'){
				string_insert(str, i, '\r');
				i++;
				changed=true; continue;
			}
		}
		//samo \r (bez \n)
		if(i>0){
			if(string_char(str, i-1)=='\r' && string_char(str, i)!='\n'){
				string_insert(str, i, '\n');
				i++;
				changed=true; continue;
			}
		}
	}
	if(nawias!=0){
		string_insert(str, str->length(), ']');
		changed=true;
		IO::geti()->log("Analiza: brak nawiasu zamykaj¹cego ] na koñcu");
	}
	save_text(str);
	return changed;
}

void App::usun_akordy(){
	string* str = load_text();
    if(last_sel_end > str->length()) last_sel_end = str->length();
    bool is_selection = true;
    if(last_sel_start>=last_sel_end){ //nie by³o zaznaczenia
		last_sel_start = 0;
		last_sel_end = str->length();
        is_selection = false;
	}
	bool nawias = false;
	for(int i=last_sel_start; i<(int)last_sel_end;i++){
		if(string_char(str, i)=='['){
			nawias = true;
			string_delete(str, i);
			last_sel_end--;
		}else if(string_char(str, i)==']'){
			nawias = false;
			string_delete(str, i);
			last_sel_end--;
		}else if(nawias){
			string_delete(str, i);
			last_sel_end--;
		}
	}
	save_text(str);
    if(is_selection){
        IO::geti()->echo("Usuniêto akordy w zaznaczeniu");
    }else{
        IO::geti()->echo("Usuniêto akordy w ca³ym pliku");
    }
}

void App::usun_wersje(){
	string* str = load_text();
	int nawias = 0;
	for(int i=0; i<(int)str->length(); i++){
		if(string_char(str, i)=='['){
			nawias = 1;
		}else if(string_char(str, i)==']'){
			nawias = 0;
		}else if(string_char(str, i+1)=='|' && nawias>0 && string_char(str, i)==' '){
			string_delete(str, i);
		}else if(string_char(str, i)=='|' && nawias>0){
			nawias++;
			string_delete(str, i);
		}else if(nawias>1){
			string_delete(str, i);
		}
	}
	save_text(str);
	IO::geti()->echo("Usuniêto alternatywne wersje akordów");
}

void App::zamien(){
    string find_edit = Controls::geti()->get_text("find_edit");
    if(find_edit == Config::i()->find_edit_placeholder) find_edit = "";
	if(find_edit.length()==0){
		IO::geti()->error("Szukany ciag jest pusty");
		return;
	}
    string replace_edit = Controls::geti()->get_text("replace_edit");
    if(replace_edit == Config::i()->replace_edit_placeholder) replace_edit = "";
	string* str = load_text();
	string nowy = "";
	int licznik = 0;
	unsigned int sel_start = 0, sel_end = str->length();
	if(get_selected(sel_start, sel_end, str)){
		sel_end += -find_edit.length() + 1;
	}else{ //szukanie w ca³ym zakresie
		sel_start = 0;
		sel_end = str->length();
	}
	for(unsigned int i=0; i<str->length(); i++){
		//pocz¹tek i koniec
		if(i<sel_start || i>=sel_end){
			nowy += string_char(str, i);
			continue;
		}
		//szukanie wzorca
		if(i <= str->length()-find_edit.length()){
			bool found = true;
			for(unsigned int j=i; j<i+find_edit.length(); j++){
				if(string_char(str, j) != find_edit[j-i]){
					found = false;
					break;
				}
			}
			if(found){
				nowy += replace_edit;
				i += find_edit.length()-1;
				licznik++;
				continue;
			}
		}
		nowy += string_char(str, i);
	}
	delete str;
	Controls::geti()->set_text("editor", nowy);
	refresh_text();
	if(licznik==0){
		IO::geti()->echo("Nie znaleziono wyst¹pieñ.");
	}else{
		stringstream ss;
		ss<<"Zamieniono "<<licznik;
		if(licznik==1) ss<<" wyst¹pienie";
		else if(licznik/10!=1&&(licznik%10==2||licznik%10==3||licznik%10==4)) ss<<" wyst¹pienia";
		else ss<<" wyst¹pieñ";
		ss<<" tekstu.";
		IO::geti()->echo(ss.str());
	}
}

bool App::znajdz_w(string* str, unsigned int start, unsigned int end, string wzor){
	for(int i=start; i<=(int)end-(int)wzor.length(); i++){
		//szukanie wzorca
		bool found = true;
		for(unsigned int j=0; j<wzor.length(); j++){
			if(to_lowercase(string_char(str, j+i)) != to_lowercase(wzor[j])){
				found = false;
				break;
			}
		}
		if(found){
			IO::geti()->echo("Znaleziono fragment");
			set_selected(i, i+wzor.length(), str);
			Controls::geti()->set_focus("find_edit");
			delete str;
			SendMessage(Controls::geti()->find("editor"), EM_SCROLLCARET, 0, 0);
			return true;
		}
	}
	return false;
}

void App::znajdz(){
    string wzor = Controls::geti()->get_text("find_edit");
    if(wzor == Config::i()->find_edit_placeholder) wzor = "";
	if(wzor.length()==0){
		IO::geti()->error("Szukany ciag jest pusty");
		return;
	}
	string *str = load_text();
    if(str->length()==0){
        delete str;
        return;
    }
    if(znajdz_w(str, last_sel_start+1, str->length(), wzor)){
        delete str;
        return;
    }
	if(last_sel_start+1+wzor.length() > str->length()) last_sel_start = str->length()-1-wzor.length();
    if(znajdz_w(str, 0, last_sel_start+1+wzor.length(), wzor)){
        delete str;
        return;
    }
	delete str;
	IO::geti()->echo("Nie znaleziono fragmentu");
}

void App::wstaw_tekst(int nrt){
	if(nrt<1 || nrt>9) return;
	string add_text = Config::geti()->tekst_wstaw[nrt-1];
	if(add_text.length()==0){
		stringstream ss;
		ss<<"Tekst nr "<<nrt<<" jest pusty";
		IO::geti()->error(ss.str());
		return;
	}
	SendMessage(Controls::geti()->find("editor"), EM_REPLACESEL, 0, (LPARAM)(add_text.c_str()));
	stringstream ss2;
	ss2<<"Wstawiono tekst nr "<<nrt<<": "<<add_text;
	IO::geti()->echo(ss2.str());
}

void App::zapisz_tekst(int nrt){
	if(nrt<1 || nrt>9) return;
    string selected = get_selected_text();
    selected = trim_1crlf(selected);
	if(selected.length()==0){
		IO::geti()->error("Nie zaznaczono tekstu do zapisania");
		return;
	}
	Config::geti()->tekst_wstaw[nrt-1] = selected;
	stringstream ss;
	ss<<"Zapisano tekst nr "<<nrt<<": "<<selected;
	IO::geti()->echo(ss.str());
}

void App::dodaj_nawias(){
	string* str = load_text();
	if(last_sel_start == last_sel_end){
		IO::geti()->error("Nie zaznaczono ¿adnego fragmentu");
		delete str;
		return;
	}
    //jeœli jest to ju¿ akord
    if(string_char(str, last_sel_start) == '[' || string_char(str, last_sel_end-1) == ']'){
        IO::geti()->error("Zaznaczony fragment jest ju¿ akordem");
		delete str;
		return;
    }
	string_insert(str, last_sel_end, ']');
	string_insert(str, last_sel_start, '[');
	last_sel_end += 2;
	save_text(str);
}

void App::dodaj_alternatywne(){
	if(Config::geti()->transposed==0){
		IO::geti()->error("Zerowe przesuniêcie akordów");
		return;
	}
	string* str = load_text();
	string nowy="", akord="";
	char ch;
	int nawias=0;
	for(unsigned int i=0; i<str->length(); i++){
		ch = string_char(str, i);
		if(ch=='['){
			nawias = 1;
			nowy += ch;
			continue;
		}
		if(ch==']'){
			if(akord.length()>0){
				 nowy += " | ";
				 nowy += transpose_chords(akord, Config::geti()->transposed);
			}
			akord = "";
			nawias = 0;
			nowy += ch;
			continue;
		}
		if(ch=='|' && nawias>=1){
			nawias++;
			nowy += ch;
			continue;
		}
		if(i<str->length()-1 && nawias>=1){
			if(ch==' ' && string_char(str, i+1)=='|'){
				nowy += ch;
				continue;
			}
		}
		if(nawias==1){
			akord += ch;
		}
		nowy += ch;
	}
    delete str;
	Controls::geti()->set_text("editor", nowy);
	refresh_text();
	IO::geti()->echo("Dodano alternatywn¹ wersjê akordów");
}

void App::save_pattern(){
    pattern.clear();
    string selected = get_selected_text();
    if(selected.length()==0){
        IO::geti()->error("Brak zaznaczenia");
        return;
    }
    int nawias_start = -1;
	for(unsigned int i=0; i<selected.length(); i++){
        if(selected.at(i)=='['){
            nawias_start = i+1;
		}else if(selected.at(i)==']' && nawias_start != -1){ //by³ nawias otwieraj¹cy
            pattern.push_back(selected.substr(nawias_start, i-nawias_start));
            nawias_start = -1; //reset nawiasu otwieraj¹cego
		}else if(string_char(&selected, i)=='\r' && string_char(&selected, i-1)!=']'){ //enter bez poprzedzaj¹cego akordu - pusty wiersz bez akordów
            pattern.push_back("");
        }
	}
    if(pattern.size()==0){
        IO::geti()->error("Brak akordów w zaznaczeniu");
    }else{
        stringstream ss;
        ss<<"Zapisano schemat "<<pattern.size()<<" akordów.";
        IO::geti()->echo(ss.str());
    }
}

void App::insert_pattern(){
    if(pattern.size()==0){
        IO::geti()->error("Brak zapisanych akordów w schemacie");
        return;
    }
    string* str = load_text();
    unsigned int sel_start, sel_end;
    if(!get_selected(sel_start, sel_end, str)){
        IO::geti()->error("Brak zaznaczenia");
        delete str;
        return;
    }
    string selected = get_selected_text();
    //zliczenie crlf
    int crlfs = 0;
    for(unsigned int i=0; i<selected.length(); i++){
        if(selected.at(i) == '\r') crlfs++;
    }
    //zgodnoœæ rozmiaru zaznaczenia
    if(crlfs != (int)pattern.size() && crlfs != (int)pattern.size()-1){
        stringstream ss;
        ss<<"Nieprawid³owy rozmiar zaznaczenia (zaznaczenie: "<<crlfs<<", schemat akordów: "<<pattern.size()<<")";
        IO::geti()->error(ss.str());
        delete str;
        return;
    }
    //wstawienie przed entery kolejnych akordów ze schematu
    crlfs = 0;
    string inserted;
    for(unsigned int i=sel_start; i<sel_end && i<str->length(); i++){
        if(str->at(i) == '\r'){
            if(pattern.at(crlfs).size()==0){
                inserted = "";
            }else{
                inserted = " ["+pattern.at(crlfs)+"]";
            }
            string_insert_string(str, i, inserted);
            //przesuniêcie przeszukiwania
            i += inserted.length();
            sel_end += inserted.length();
            //zmiana zaznaczenia
            last_sel_end += inserted.length();
            crlfs++;
        }
    }
    if(crlfs == (int)pattern.size()-1){ // zaznaczenie ostatniego wiersza bez entera
        if(pattern.at(crlfs).size()==0){
            inserted = "";
        }else{
            inserted = " ["+pattern.at(crlfs)+"]";
        }
        string_insert_string(str, sel_end, inserted);
        //zmiana zaznaczenia
        last_sel_end += inserted.length();
    }
    save_text(str);
    stringstream ss;
    ss<<"Wstawiono schemat "<<pattern.size()<<" akordów.";
    IO::geti()->echo(ss.str());
}
