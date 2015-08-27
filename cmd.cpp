#include "app.h"

void App::exec_cmd(){
    /*
	char *str2 = new char[256];
	GetWindowText(hctrl[0],str2,256);
	if(strlen(str2)==0){
		SetWindowText(hctrl[0],last_cmd.c_str());
		return;
	}
	SetWindowText(hctrl[0],"");
	string str2_s = str2;
	exec_cmd(str2_s);
	last_cmd=str2_s;
	delete[] str2;
    */
}

void App::exec_cmd(string str2){
    /*
	ss_clear(ss);
	ss<<"> "<<str2;
	echo(ss.str());
	int firstspace = -1;
	string cmd1 = "", cmd2 = "";
	for(unsigned int i=0; i<str2.length(); i++){
		if(str2[i]==' '){
			firstspace = i;
			break;
		}
	}
	if(firstspace==-1) firstspace = str2.length();
	for(int i=0; i<firstspace; i++){
		cmd1+=str2[i];
	}
	for(unsigned int i=firstspace+1; i<str2.length(); i++){
		cmd2+=str2[i];
	}
	if(str2=="log"){
		ShellExecute(0,"open","log.txt","",0,SW_SHOW);
	}else if(str2=="config"){
		ShellExecute(0,"open","conf.ini","",0,SW_SHOW);
	}else if(str2=="reload_config"){
		load_config();
		ss_clear(ss);
		ss<<autoscroll_interval;
		SetWindowText(hctrl[19],ss.str().c_str());
		ss_clear(ss);
		ss<<autoscroll_wait;
		SetWindowText(hctrl[20],ss.str().c_str());
		fontface=config_fontface;
		fontsize=config_fontsize2;
		change_font(hctrl[2]);
		echo("Ustawienia wczytane");
	}else if(str2=="dir"){
		ShellExecute(0,"open",".","",0,SW_SHOW);
	}else if(str2=="exit"){
		DestroyWindow(hwnd);
	}else if(str2=="opened_file"||str2=="o"){
		SetWindowText(hctrl[0],opened_file.c_str());
	}else if(str2=="reload"){
		open_file(opened_file);
	}else if(str2=="font++"){
		change_font_size(+1);
	}else if(str2=="font--"){
		change_font_size(-1);
	}else if(str2=="autoscroll"){
		autoscroll_on();
	}else if(str2=="autoscroll_off"){
		autoscroll_off();
	}else if(str2=="siema"){
		message("No siema :)");
		SetFocus(hctrl[0]);
	}else if(str2=="new"){
		new_file();
    }else if(str2=="format"){
        refresh_text();
    }else if(str2=="remove_chords"){
        usun_akordy();
    }else if(str2=="remove_alt"){
        usun_wersje();
    }else if(str2=="help"){
		ss_clear(ss);
		ss<<"Skróty klawiszowe:"<<endl;
		ss<<"F1: przewiñ na pocz¹tek"<<endl;
		ss<<"F2: przewiñ w górê"<<endl;
		ss<<"F3: przewiñ w dó³"<<endl;
		ss<<"F4: dodaj nawiasy []"<<endl;
		ss<<"F5: zmniejsz szybkoœæ przewijania"<<endl;
		ss<<"F6: zwiêksz szybkoœæ przewijania"<<endl;
		ss<<"F7: uruchom autoscroll"<<endl;
		ss<<"F8: uruchom autoscroll bez opóŸnienia"<<endl;
		ss<<"F9: poka¿ / schowaj pasek przycisków"<<endl;
		ss<<"F11: pe³ny ekran"<<endl;
		ss<<"Ctrl + [`]: wiersz poleceñ"<<endl;
		ss<<"Ctrl + [+]: zwiêksz czcionkê"<<endl;
		ss<<"Ctrl + [-]: zmniejsz czcionkê"<<endl;
		ss<<"Ctrl + [lewo]: transponuj akordy -1"<<endl;
		ss<<"Ctrl + [prawo]: transponuj akordy +1"<<endl;
		ss<<"Ctrl + [T]: szybka zamiana tekstu"<<endl;
		ss<<"Ctrl + [0]: przywróc oryginaln¹ tonacjê"<<endl;
		ss<<"Ctrl + [1-9]: zapisz zaznaczony tekst do wstawienia"<<endl;
		ss<<"Alt + [1-9]: wstaw zapisany tekst"<<endl;
		ss<<endl<<"Polecenia:"<<endl;
		ss<<"log - poka¿ dziennik zdarzeñ"<<endl;
		ss<<"config - otwórz plik konfiguracyjny"<<endl;
		ss<<"reload_config - wczytaj ustawienia"<<endl;
		ss<<"dir - otwórz folder roboczy"<<endl;
        ss<<"opened_file, o - wyœwietl nazwê otwartego pliku"<<endl;
        ss<<"remove_chords - usuñ akordy z zaznaczenia (lub ca³oœci)"<<endl;
        ss<<"remove_alt - usuñ alternatywne akordy"<<endl;
		ss<<"font [czcionka] - zmieñ czcionkê"<<endl;
		ss<<"fontsize [rozmiar] - zmieñ rozmiar czcionki"<<endl;
        ss<<"alt [shift] - dodaj alternatywn¹ tonacjê"<<endl;
        ss<<"transpose [shift] - transponuj tonacjê akordów"<<endl;
        ss<<"transpose 0 - przywróæ oryginaln¹ tonacjê"<<endl;
		message(ss.str());
		SetFocus(hctrl[0]);
	}else if(cmd1=="font"){
		if(cmd2.length()>0){
			fontface = cmd2;
			change_font(hctrl[2]);
		}
		ss_clear(ss);
		ss<<"Czcionka: "<<fontface;
		echo(ss.str());
	}else if(cmd1=="fontsize"){
		if(cmd2.length()>0){
			fontsize=atoi(cmd2.c_str());
			if(fontsize<1) fontsize=1;
			change_font(hctrl[2]);
		}
		ss_clear(ss);
		ss<<"Rozmiar czcionki: "<<fontsize;
		echo(ss.str());
    }else if(cmd1=="alt"){
        int shift=0, old_transposed;
        if(cmd2.length()>0){
            shift = atoi(cmd2.c_str());
        }
        if(shift==0){
            echo("! B³¹d: Zerowe przesuniêcie akordów");
            return;
        }
        old_transposed = transposed;
        transposed = shift;
        dodaj_alternatywne();
        transposed = old_transposed;
        ss_clear(ss);
        ss<<"Dodano alternatywn¹ wersjê przesuniêt¹ o: ";
        if(shift>0) ss<<"+";
        ss<<shift;
        echo(ss.str());
    }else if(cmd1=="transpose"){
        int shift=0;
        if(cmd2.length()>0){
            shift = atoi(cmd2.c_str());
        }
        if(shift==0) shift=-transposed;
        transpose(shift);
    }else{
		ss_clear(ss);
		ss<<"! B³¹d: Nie znaleziono polecenia: "<<str2;
		echo(ss.str());
	}
    */
}
