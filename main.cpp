#include "app.h"
#include "system.h"

using namespace std;

LRESULT CALLBACK windowProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);

App *app = new App();

LRESULT CALLBACK wndproc_new(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam){
	return app->subclass_wndproc_new(hwnd, message, wParam, lParam);
}

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow){
	app->hInst = &hInstance;
	app->windowProc = windowProc;
	app->wndproc_new = wndproc_new;
    app->check_instance(Config::geti()->program_name);
	WNDCLASS windowClass;
	windowClass.lpfnWndProc = windowProc;
	windowClass.style = CS_HREDRAW | CS_VREDRAW;
	windowClass.hInstance = hInstance;
	windowClass.hCursor = LoadCursor(NULL, IDC_ARROW);
	windowClass.hIcon = LoadIcon(hInstance, MAKEINTRESOURCE(2)); //LoadIcon(NULL, IDI_APPLICATION);
	windowClass.hbrBackground = (HBRUSH)COLOR_BTNSHADOW;
	windowClass.cbClsExtra = 0;
	windowClass.cbWndExtra = 0;
	windowClass.lpszClassName = Config::geti()->program_name.c_str();
	windowClass.lpszMenuName  = NULL;
	if(!RegisterClass(&windowClass)){
		System::geti()->message_box("B³¹d krytyczny!", "RegisterClass failed");
		return 0;
	}
	HWND window = CreateWindowEx(0, Config::geti()->program_name.c_str(), Config::geti()->program_name.c_str(), WS_OVERLAPPEDWINDOW, CW_USEDEFAULT, CW_USEDEFAULT, Config::geti()->window_w+16, Config::geti()->window_h+38, NULL, NULL, hInstance, NULL);
	if(!window){
		System::geti()->message_box("B³¹d krytyczny!", "window NULL pointer");
		return 0;
	}
	ShowWindow(window, nCmdShow);
	UpdateWindow(window);
	MSG message;
	while(GetMessage(&message, NULL, 0, 0)){
		TranslateMessage(&message);
		DispatchMessage(&message);
	}
	return message.wParam;
}

LRESULT CALLBACK windowProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam){
	switch(message){
		case WM_CREATE:{
			app->event_create(&hwnd);
		}break;
		case WM_COMMAND:{
			app->event_button(wParam);
		}break;
		case WM_DROPFILES:{
			HDROP fDrop = (HDROP)wParam;
			char* fName = new char[512];
			DragQueryFile(fDrop,0,fName,512);
			DragFinish(fDrop);
			app->event_dropfiles(fName);
			delete[] fName;
		}break;
		case WM_SIZE:{
			app->event_resize();
		}break;
		case WM_TIMER:{
			app->event_timer();
		}break;
		case WM_KEYDOWN:{
			app->event_keydown(wParam);
		}
		case WM_SYSKEYDOWN:{
			app->event_syskeydown(wParam);
		}
        case WM_SYSCOMMAND:{
            if(wParam==SC_SCREENSAVE){
                app->event_screensave();
            }
        }break;
        case 0x0319:{ //APP_COMMAND
            app->event_appcommand(wParam,lParam);
        }break;
		case WM_DESTROY:{
			delete app;
			return 0;
		}break;
	}
	return DefWindowProc(hwnd, message, wParam, lParam);
}
