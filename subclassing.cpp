#include "app.h"
#include <richedit.h>

LRESULT CALLBACK App::subclass_wndproc_new(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam){
	//odnalezienie kontrolki
    Control* kontrolka = NULL;
	for(unsigned int i=0; i<Controls::geti()->controls.size(); i++){
        if(Controls::geti()->controls.at(i)->handle == hwnd){
            kontrolka = Controls::geti()->controls.at(i);
            break;
        }
	}
    if(kontrolka==NULL){
        IO::geti()->error("Subclassing: Nie znaleziono kontrolki o podanym uchwycie");
        return 0;
    }
    string nazwa = kontrolka->name;
    //globalne skróty klawiszowe
    if(message == WM_KEYDOWN){
        if(wParam==VK_F1||wParam==VK_F2||wParam==VK_F3||wParam==VK_F5||wParam==VK_F6||wParam==VK_F7||wParam==VK_F8||wParam==VK_F9||wParam==VK_F10||wParam==VK_F11){
            CallWindowProc(windowProc, hwnd, message, wParam, lParam); //przekazanie wy¿ej do mainwindow
            return 0; //przechwycenie
        }
        //wyjœcie z fullscreena
        if(wParam==VK_ESCAPE && Config::geti()->fullscreen_on){
            CallWindowProc(windowProc, hwnd, message, wParam, lParam); //przekazanie wy¿ej do mainwindow
            return 0; //przechwycenie
        }
        //ctrl
        if(is_control_pressed()){
            if(wParam=='S'||wParam=='F'||wParam=='N'||wParam=='B'||wParam==VK_ADD||wParam==VK_SUBTRACT||wParam==VK_LEFT||wParam==VK_RIGHT||wParam=='0'||wParam==VK_NUMPAD0||wParam==VK_OEM_3){
                CallWindowProc(windowProc, hwnd, message, wParam, lParam); //przekazanie wy¿ej do mainwindow
                return 0; //przechwycenie
            }
        }
    }
    if(message == WM_SYSKEYDOWN){
        if(wParam==VK_F10){
            //zamiana WM_SYSKEYDOWN na WM_KEYDOWN
            subclass_wndproc_new(hwnd, WM_KEYDOWN, wParam, lParam);
            return 0;
        }
    }
    //globalne przechwycenie wygaszacza
    if(message == WM_SYSCOMMAND){
        if(wParam == SC_SCREENSAVE){
            CallWindowProc(windowProc, hwnd, message, wParam, lParam); //przekazanie wy¿ej do mainwindow
            return 0; //przechwycenie
        }
    }
	//nowe procedury kontrolek
    if(nazwa=="filename_edit"){
		switch(message){
			case WM_CHAR:{
				if(wParam==VK_ESCAPE){
					Controls::geti()->set_focus("editor");
					return 0;
				}else if(wParam==VK_RETURN){
                    if(!Config::geti()->cmd_show){
                        cmd_switch(1);
                    }
                    Controls::geti()->set_focus("cmd");
                    Controls::geti()->select_all("cmd");
                    return 0;
                }
			}break;
		}
	}
    if(nazwa=="cmd"){
		switch(message){
			case WM_CHAR:{
				if(wParam==VK_ESCAPE){
					Controls::geti()->set_focus("editor");
					return 0;
				}else if(wParam==VK_RETURN){
					exec_cmd();
					return 0; //przechwycenie
				}
			}break;
			case WM_KEYDOWN:{
				if(wParam==VK_UP){
                    Controls::geti()->set_text("cmd", last_cmd);
					SendMessage(Controls::geti()->find("cmd"), EM_SETSEL, last_cmd.length(), last_cmd.length());
					return 0;
				}
				if(wParam==VK_DOWN){
					Controls::geti()->set_text("cmd", "");
					return 0;
				}
			}break;
		}
	}
	if(nazwa=="editor"){ //edytor
		switch(message){
			case WM_SYSKEYDOWN:{
                if(wParam>='1' && wParam<='9'){
                    //zamiana WM_SYSKEYDOWN na WM_KEYDOWN
                    subclass_wndproc_new(hwnd, WM_KEYDOWN, wParam, lParam);
                    return 0;
				}
			}break;
			case WM_KEYDOWN:{
				if(is_control_pressed()){ //ctrl
					if(wParam=='C'){
						copy_text();
						return 0;
					}else if(wParam=='A'){
                        select_all();
                        return 0;
					}else if(wParam=='R'){
                        refresh_text();
                        return 0;
					}else if(wParam=='T'){
						quick_replace();
						return 0;
					}else if(wParam=='Q'){
						usun_akordy();
						return 0;
					}else if(wParam=='W'){
						save_pattern();
						return 0;
					}else if(wParam=='E'){
                        insert_pattern();
						return 0;
					}else if(wParam=='D'){
                        dodaj_nawias();
						return 0;
					}else if(wParam>='1' && wParam<='9'){
						zapisz_tekst(wParam-'1'+1);
						return 0;
					}
				}
				if(is_alt_pressed()){ //alt
					if(wParam>='1' && wParam<='9'){
						wstaw_tekst(wParam-'1'+1);
						return 0;
					}
				}
			}break;
			case WM_CHAR:{
				if(wParam == ']'){
					string add_text = "]";
					SendMessage(Controls::geti()->find("editor"), EM_REPLACESEL, 0, (LPARAM)add_text.c_str()); //wstawienie znaku
					refresh_text(); //odœwie¿enie tekstu
                    //zmiana koloru po nawiasie na kolor tekstu
                    CHARFORMAT cf_text;
                    cf_text.cbSize = sizeof(cf_text);
                    SendMessage(Controls::geti()->find("editor"), EM_GETCHARFORMAT, SCF_DEFAULT, (LPARAM)&cf_text);
                    //kolor tekstu + brak pogrubienia
                    cf_text.dwMask = CFM_COLOR | CFM_BOLD;
                    cf_text.crTextColor = RGB(Config::i()->color_text[0], Config::i()->color_text[1], Config::i()->color_text[2]);
                    cf_text.dwEffects = 0;
                    SendMessage(Controls::geti()->find("editor"), EM_SETCHARFORMAT, SCF_SELECTION, (LPARAM)&cf_text);
                    return 0;
				}
			}break;
			case WM_MOUSEWHEEL:{
                int zdelta = GET_WHEEL_DELTA_WPARAM(wParam);
				if(LOWORD(wParam)==MK_CONTROL){ //z controlem
                    //zmiana czcionki
					if(zdelta>=0){
						change_font_size(+1);
					}else{
						change_font_size(-1);
					}
                    return 0;
				}else{
                    //scrollowanie edytora
                    int scroll_step = Config::geti()->editor_fontsize + 2;
                    if(zdelta>=0){
						change_scroll(-scroll_step);
					}else{
						change_scroll(+scroll_step);
					}
                    return 0;
                }
			}break;
		}
	}
	if(nazwa=="find_edit"){ //szukany ci¹g znaków
		switch(message){
			case WM_SETFOCUS:{
                if(Controls::i()->get_text("find_edit")==Config::i()->find_edit_placeholder){
                    Controls::i()->set_text("find_edit", "");
                }
				IO::geti()->echo("(Wpisz szukany ci¹g znaków, Enter - szukaj)");
			}break;
            case WM_KILLFOCUS:{
                if(Controls::i()->get_text("find_edit").length()==0){
                    Controls::i()->set_text("find_edit", Config::i()->find_edit_placeholder);
                }
            }break;
			case WM_CHAR:{
				if(wParam==VK_RETURN){
					znajdz();
					return 0;
				}
				if(wParam==VK_TAB){
					Controls::geti()->set_focus("replace_edit");
					return 0;
				}
			}break;
		}
	}
	if(nazwa=="replace_edit"){ //tekst do zamiany
		switch(message){
			case WM_SETFOCUS:{
                if(Controls::i()->get_text("replace_edit")==Config::i()->replace_edit_placeholder){
                    Controls::i()->set_text("replace_edit", "");
                }
                    IO::geti()->echo("(Wpisz tekst do zamiany, Enter - zamieñ w zaznaczeniu lub ca³ym pliku)");
                }break;
            case WM_KILLFOCUS:{
                if(Controls::i()->get_text("replace_edit").length()==0){
                    Controls::i()->set_text("replace_edit", Config::i()->replace_edit_placeholder);
                }
            }break;
			case WM_CHAR:{
				if(wParam==VK_RETURN){
					zamien();
					return 0;
				}
				if(wParam==VK_TAB){
					Controls::geti()->set_focus("find_edit");
					return 0;
				}
			}break;
		}
	}
	if(nazwa=="autoscroll_interval"){ //autoscroll_interval
		switch(message){
			case WM_SETFOCUS:{
				IO::geti()->echo("(Czas miêdzy kolejnymi przebiegami autoscrolla - wyra¿ony w milisekundach)");
			}break;
			case WM_CHAR:{
				if(wParam==VK_RETURN){
					autoscroll_nowait(0);
					return 0;
				}
			}break;
		}
	}
	if(nazwa=="autoscroll_wait"){ //autoscroll_wait
		switch(message){
			case WM_SETFOCUS:{
				IO::geti()->echo("(Czas opóŸnienia przed autoscrollem - wyra¿ony w sekundach)");
			}break;
			case WM_CHAR:{
				if(wParam==VK_RETURN){
					autoscroll_on();
					return 0;
				}
			}break;
		}
	}
	return CallWindowProc(kontrolka->wndproc_old, hwnd, message, wParam, lParam);
}

void App::subclass(Control* kontrolka){
    if(kontrolka==NULL) return;
    kontrolka->wndproc_old = (WNDPROC) SetWindowLong(kontrolka->handle, GWL_WNDPROC, (LONG)wndproc_new);
}

void App::subclass(string name){
    subclass(Controls::geti()->find_control(name));
}

void App::un_subclass(Control* kontrolka){
    if(kontrolka==NULL) return;
	if(kontrolka->wndproc_old==NULL) return;
	SetWindowLong(kontrolka->handle, GWL_WNDPROC, (LONG)kontrolka->wndproc_old);
}

void App::un_subclass(string name){
    un_subclass(Controls::geti()->find_control(name));
}
