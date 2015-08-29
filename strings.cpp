#include "strings.h"

char string_char(string* str, int pos){
	if(pos<0 || pos>=(int)str->length()) return 0;
	return str->at(pos);
}

void string_char_set(string* str, int pos, char c){
    if(pos<0 || pos>=(int)str->length()) return;
	str->at(pos) = c;
}

void string_delete(string*& str, int &pos){
	if(pos<0 || pos>=(int)str->length()) return;
    str->erase(pos, 1);
	pos--;
	//if(pos<0) pos=0;
}

void string_insert(string*& str, int pos, char c){
	if(pos<0 || pos>(int)str->length()) return;
    str->insert(str->begin()+pos, c);
}


string string_cutfromend(string s, unsigned int from_end){
    if(s.length() < from_end) from_end = s.length();
    s = s.substr(0, s.length()-from_end);
    return s;
}


string trim_spaces(string s){
    //obciêcie spacji na koñcu
    while(s.length() > 0  && s[s.length()-1] == ' '){
        s = s.substr(0, s.length()-1);
    }
    //obciêcie spacji na pocz¹tku
    while(s.length() > 0  && s[0] == ' '){
        s = s.substr(1);
    }
    return s;
}

string trim_1crlf(string s){
    //jeœli zawiera tylko 1 enter na koñcu
    if(s.length()>1){
        if(s[s.length()-1]=='\n' || s[s.length()-1]=='\r'){
            bool znaleziono = false;
            for(unsigned int i=0; i<s.length()-1; i++){
                if(s[i]==s[s.length()-1]){ //znak tego samego typu jak na koñcu (\n lub \r)
                    znaleziono = true;
                    break;
                }
            }
            if(!znaleziono){
                s = s.substr(0, s.length()-1);
            }
        }
    }
    return s;
}

string trim_quotes(string s){
    if(s.length()>=3){
        //jeœli cudzys³owy s¹ na poczatku i na koñcu
        if(s[0]=='\"' && s[s.length()-1]=='\"'){
            //jeœli w ca³ym stringu znajduj¹ siê tylko 2 cudzys³owy
            if(s.substr(1, s.length()-2).find('\"')==string::npos){
                s = s.substr(1, s.length()-2); //usuñ je
            }
        }
    }
    return s;
}

char to_lowercase(char in){
	if(in>='A' && in<='Z') return in-'A'+'a';
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
