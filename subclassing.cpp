#include "app.h"
#include <richedit.h>

LRESULT CALLBACK App::subclass_wndproc_new(HWND hwdp, UINT message, WPARAM wParam, LPARAM lParam){
	//rozpoznanie numeru kontrolki
	int ctrl=-1;
	for(int i=0; i<ctrls_num; i++){
		if(hwdp==hctrl[i]){
			ctrl=i;
			break;
		}
	}
	if(ctrl==-1) return 0;
	//nowe procedury kontrolek
	if(ctrl==0){ //wiersz poleceñ
		switch(message){
			case WM_CHAR:{
				if(wParam==VK_ESCAPE){
					SetFocus(hctrl[2]);
					return 0;
				}
				if(wParam==VK_RETURN){
					exec_cmd();
					return 0; //przechwycenie
				}
			}break;
			case WM_KEYDOWN:{
				if(wParam==VK_F5||wParam==VK_F6||wParam==VK_F7||wParam==VK_F8||wParam==VK_F11){
					CallWindowProc(windowProc, hwnd, message, wParam, lParam);
					return 0;
				}
				if((GetAsyncKeyState(VK_CONTROL)&0x8000)&&!(GetAsyncKeyState(VK_MENU)&0x8000)){ //ctrl
					CallWindowProc(windowProc, hwnd, message, wParam, lParam); //przekazanie wy¿ej
					return 0; //przechwycenie
				}
				if(wParam==VK_UP){
					SetWindowText(hctrl[0],last_cmd.c_str());
					SendMessage(hctrl[0], EM_SETSEL, last_cmd.length(), last_cmd.length());
					return 0;
				}
				if(wParam==VK_DOWN){
					SetWindowText(hctrl[0],"");
					return 0;
				}
			}break;
		}
	}
	if(ctrl==2){ //edytor
		switch(message){
			case WM_SYSCOMMAND:{
				if(wParam==SC_SCREENSAVE){
					CallWindowProc(windowProc, hwnd, message, wParam, lParam); //przekazanie wy¿ej
					return 0; //przechwycenie
				}
			}break;
			case WM_SYSKEYDOWN:{
				if(wParam==VK_F10){
				  subclass_wndproc_new(hctrl[2], WM_KEYDOWN, wParam, lParam);
				  return 0;
				}
				if(wParam>='1'&&wParam<='9'){
				  subclass_wndproc_new(hctrl[2], WM_KEYDOWN, wParam, lParam);
					return 0;
				}
			}break;
			case WM_KEYDOWN:{
				if(wParam==VK_F4){
					dodaj_nawias();
					return 0;
				}
				if(wParam==VK_F1||wParam==VK_F2||wParam==VK_F3||wParam==VK_F5||wParam==VK_F6||wParam==VK_F7||wParam==VK_F8||wParam==VK_F9||wParam==VK_F11){
					CallWindowProc(windowProc, hwnd, message, wParam, lParam);
					return 0;
				}
				if((GetAsyncKeyState(VK_CONTROL)&0x8000)&&!(GetAsyncKeyState(VK_MENU)&0x8000)){ //ctrl
					if(wParam=='C'){
						copy_text();
						return 0;
					}
					if(wParam=='T'){
						quick_replace();
						return 0;
					}
					if(wParam>='1'&&wParam<='9'){
						zapisz_tekst(wParam-'1'+1);
						return 0;
					}
					if(wParam=='A'||wParam=='S'||wParam=='R'||wParam=='F'||wParam==VK_ADD||wParam==VK_SUBTRACT||wParam==VK_OEM_3||wParam==VK_LEFT||wParam==VK_RIGHT||wParam=='0'||wParam==VK_NUMPAD0){
						CallWindowProc(windowProc, hwnd, message, wParam, lParam);
						return 0;
					}
				}
				if(((GetAsyncKeyState(VK_MENU)&0x8000)&&!(GetAsyncKeyState(VK_CONTROL)&0x8000))||(GetAsyncKeyState(VK_RMENU)&0x8000)){ //alt
					if(wParam>='1'&&wParam<='9'){
						wstaw_tekst(wParam-'1'+1);
						return 0;
					}
				}
			}break;
			case WM_CHAR:{
				if(wParam==']'){
					string add_text = "]";
					SendMessage(hctrl[2], EM_REPLACESEL, 0, (LPARAM)add_text.c_str());
					refresh_text();
					CHARFORMAT cf;
                    cf.cbSize = sizeof(cf);
					SendMessage(hctrl[2],EM_GETCHARFORMAT,SCF_SELECTION,(LPARAM)&cf);
					cf.dwMask = CFM_COLOR;
					cf.crTextColor = RGB(255,255,255);
					cf.dwEffects = 0;
					SendMessage(hctrl[2],EM_SETCHARFORMAT,SCF_SELECTION,(LPARAM)&cf);
					cf.dwMask = CFM_BOLD;
					cf.dwEffects = 0;
					SendMessage(hctrl[2],EM_SETCHARFORMAT,SCF_SELECTION,(LPARAM)&cf);
					return 0;
				}
			}break;
			case WM_MOUSEWHEEL:{
				if(LOWORD(wParam)==MK_CONTROL){
					int zdelta = GET_WHEEL_DELTA_WPARAM(wParam);
					if(zdelta>=0){
						change_font_size(+1);
					}else{
						change_font_size(-1);
					}
				}
			}break;
		}
	}
	if(ctrl==22){ //ukrycie paska
		switch(message){
			case WM_KEYDOWN:{
				CallWindowProc(windowProc, hwnd, message, wParam, lParam); //przekazanie wy¿ej
				return 0;
			}break;
		}
	}
	if(ctrl==9){ //szukany ci¹g znaków
		switch(message){
			case WM_SETFOCUS:{
				echo("(Szukany ci¹g znaków)");
			}break;
			case WM_CHAR:{
				if(wParam==VK_RETURN){
					znajdz();
					return 0;
				}
				if(wParam==VK_TAB){
					SetFocus(hctrl[10]);
					return 0;
				}
			}break;
		}
	}
	if(ctrl==10){ //tekst do zamiany
		switch(message){
			case WM_SETFOCUS:{
				echo("(Tekst po zamianie)");
			}break;
			case WM_CHAR:{
				if(wParam==VK_RETURN){
					zamien();
					return 0;
				}
				if(wParam==VK_TAB){
					SetFocus(hctrl[9]);
					return 0;
				}
			}break;
		}
	}
	if(ctrl==19){ //autoscroll_interval
		switch(message){
			case WM_SETFOCUS:{
				echo("(Czas miêdzy kolejnymi przebiegami autoscrolla - wyra¿ony w milisekundach)");
			}break;
			case WM_CHAR:{
				if(wParam==VK_RETURN){
					autoscroll_nowait(0);
					return 0;
				}
			}break;
		}
	}
	if(ctrl==20){ //autoscroll_wait
		switch(message){
			case WM_SETFOCUS:{
				echo("(Czas opóŸnienia przed autoscrollem - wyra¿ony w sekundach)");
			}break;
			case WM_CHAR:{
				if(wParam==VK_RETURN){
					autoscroll_on();
					return 0;
				}
			}break;
		}
	}
	return CallWindowProc(wndproc_old[ctrl], hwdp, message, wParam, lParam);
}

void App::subclass(int ctrl){
	wndproc_old[ctrl] = (WNDPROC) SetWindowLong(hctrl[ctrl],GWL_WNDPROC,(LONG)wndproc_new);
}

void App::un_subclass(int ctrl){
	if(wndproc_old[ctrl]==NULL) return;
	SetWindowLong(hctrl[ctrl],GWL_WNDPROC,(LONG)wndproc_old[ctrl]);
}
