#include "app.h"

void App::exec_cmd(){
    string input_cmd = Controls::geti()->get_text("cmd");
	if(input_cmd.length()==0){
        Controls::geti()->set_text("cmd", last_cmd);
		return;
	}
    Controls::geti()->set_text("cmd", "");
	exec_cmd(input_cmd);
	last_cmd = input_cmd;
}

void App::exec_cmd(string cmd){
    IO::geti()->echo("> "+cmd);
    //podzia� na g��wne polecenie i parametry
    vector<string> cmds;
    unsigned int start = 0;
    for(unsigned int i=0; i<=cmd.length(); i++){
        if(i==cmd.length() || cmd[i]==' '){
            if(i-start>0 && start<cmd.length()){
                cmds.push_back(cmd.substr(start, i-start));
            }
            start = i+1;
        }
    }
    stringstream ss;
    string cmd1 = cmds.size()>=1 ? cmds.at(0) : "";
    string cmd2 = cmds.size()>=2 ? cmds.at(1) : "";
    //wykonanie polece�
	if(cmd1=="log"){
		ShellExecute(0, "open", Config::geti()->log_filename.c_str(), "", 0, SW_SHOW);
	}else if(cmd1=="config"){
        if(cmd2=="reload"){
            Config::geti()->load_from_file();
            Controls::geti()->set_text("autoscroll_interval", Config::geti()->autoscroll_interval);
            Controls::geti()->set_text("autoscroll_wait", Config::geti()->autoscroll_wait);
            controls_fonts_set();
        }else{
            ShellExecute(0,"open",Config::geti()->config_filename.c_str(),"",0,SW_SHOW);
        }
	}else if(cmd1=="dir"){
		ShellExecute(0, "open", ".", "", 0, SW_SHOW);
	}else if(cmd1=="exit"){
		DestroyWindow(main_window);
	}else if(cmd1=="opened_file"||cmd1=="o"){
        Controls::geti()->set_text("filename_edit", Config::geti()->opened_file);
	}else if(cmd1=="reload"){
		open_chords_file(Config::geti()->opened_file);
	}else if(cmd1=="font++"){
		change_font_size(+1);
	}else if(cmd1=="font--"){
		change_font_size(-1);
	}else if(cmd1=="autoscroll"){
		autoscroll_on();
	}else if(cmd1=="autoscroll_off"){
		autoscroll_off();
	}else if(cmd1=="siema"){
		IO::geti()->message_box("Wiadomo��","No siema :)");
        Controls::geti()->set_focus("cmd");
	}else if(cmd1=="new"){
		new_file();
    }else if(cmd1=="format"){
        refresh_text();
    }else if(cmd1=="remove_chords"){
        usun_akordy();
    }else if(cmd1=="remove_alt"){
        usun_wersje();
    }else if(cmd1=="help"){
        show_help();
		Controls::geti()->set_focus("cmd");
	}else if(cmd1=="fontface"){
		if(cmd2.length()>0){
			Config::geti()->editor_fontface = cmd2;
			Controls::geti()->set_font("editor", Config::geti()->editor_fontface, Config::geti()->editor_fontsize);
		}
		ss_clear(ss);
		ss<<"Czcionka edytora: "<<Config::geti()->editor_fontface;
		IO::geti()->echo(ss.str());
	}else if(cmd1=="fontsize"){
		if(cmd2.length()>0){
			Config::geti()->editor_fontsize = atoi(cmd2.c_str());
			if(Config::geti()->editor_fontsize<1) Config::geti()->editor_fontsize = 1;
			Controls::geti()->set_font("editor", Config::geti()->editor_fontface, Config::geti()->editor_fontsize);
		}
		ss_clear(ss);
		ss<<"Rozmiar czcionki edytora: "<<Config::geti()->editor_fontsize;
		IO::geti()->echo(ss.str());
    }else if(cmd1=="alt"){
        int shift=0, old_transposed;
        if(cmd2.length()>0){
            shift = atoi(cmd2.c_str());
        }
        if(shift==0){
            IO::geti()->error("Zerowe przesuni�cie akord�w");
            return;
        }
        old_transposed = Config::geti()->transposed;
        Config::geti()->transposed = shift;
        dodaj_alternatywne();
        Config::geti()->transposed = old_transposed;
        ss_clear(ss);
        ss<<"Dodano alternatywn� wersj� przesuni�t� o: ";
        if(shift>0) ss<<"+";
        ss<<shift;
        IO::geti()->echo(ss.str());
    }else if(cmd1=="transpose"){
        int shift = 0;
        if(cmd2.length()>0){
            shift = atoi(cmd2.c_str());
        }
        if(shift==0) shift = - Config::geti()->transposed;
        transpose(shift);
    }else{
		IO::geti()->error("Nie znaleziono polecenia: "+cmd);
	}
}

void App::show_help(){
    stringstream ss;
    ss<<"Skr�ty klawiszowe:"<<endl;
    ss<<"F1: przewi� na pocz�tek"<<endl;
    ss<<"F2: przewi� w g�r�"<<endl;
    ss<<"F3: przewi� w d�"<<endl;
    ss<<"F4: dodaj nawiasy []"<<endl;
    ss<<"F5: zmniejsz szybko�� przewijania"<<endl;
    ss<<"F6: zwi�ksz szybko�� przewijania"<<endl;
    ss<<"F7: uruchom autoscroll"<<endl;
    ss<<"F8: uruchom autoscroll bez op�nienia"<<endl;
    ss<<"F9: poka� / schowaj pasek przycisk�w"<<endl;
    ss<<"F11: pe�ny ekran"<<endl;
    ss<<"Ctrl + [`]: wiersz polece�"<<endl;
    ss<<"Ctrl + [+]: zwi�ksz czcionk�"<<endl;
    ss<<"Ctrl + [-]: zmniejsz czcionk�"<<endl;
    ss<<"Ctrl + [lewo]: transponuj akordy -1"<<endl;
    ss<<"Ctrl + [prawo]: transponuj akordy +1"<<endl;
    ss<<"Ctrl + [T]: szybka zamiana tekstu"<<endl;
    ss<<"Ctrl + [0]: przywr�c oryginaln� tonacj�"<<endl;
    ss<<"Ctrl + [1-9]: zapisz zaznaczony tekst do wstawienia"<<endl;
    ss<<"Alt + [1-9]: wstaw zapisany tekst"<<endl;
    ss<<endl<<"Polecenia:"<<endl;
    ss<<"log - poka� dziennik zdarze�"<<endl;
    ss<<"config - otw�rz plik konfiguracyjny"<<endl;
    ss<<"config reload - wczytaj ustawienia"<<endl;
    ss<<"dir - otw�rz folder roboczy"<<endl;
    ss<<"opened_file, o - wy�wietl nazw� otwartego pliku"<<endl;
    ss<<"remove_chords - usu� akordy z zaznaczenia (lub ca�o�ci)"<<endl;
    ss<<"remove_alt - usu� alternatywne akordy"<<endl;
    ss<<"fontface [czcionka] - zmie� czcionk� edytora"<<endl;
    ss<<"fontsize [rozmiar] - zmie� rozmiar czcionki edytora"<<endl;
    ss<<"alt [shift] - dodaj alternatywn� tonacj�"<<endl;
    ss<<"transpose [shift] - transponuj tonacj� akord�w"<<endl;
    ss<<"transpose 0 - przywr�� oryginaln� tonacj�"<<endl;
    IO::geti()->message_box("Pomoc",ss.str());
}
