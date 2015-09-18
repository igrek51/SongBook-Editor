#include "app.h"
#include "strings.h"
#include <richedit.h>
#include <fstream>

void App::update_title(){
	stringstream ss;
	ss<<Config::geti()->program_name<<" v"<<version<<": ";
	if(Config::geti()->opened_file.length()==0){
		ss<<"[Nowy plik]";
	}else{
		ss<<Config::geti()->opened_file;
	}
	SetWindowText(main_window, ss.str().c_str());
}

void App::controls_fonts_set(){
    for(unsigned int i=0; i<Controls::geti()->controls.size(); i++){
        string fontface = Config::geti()->buttons_fontface;
        int fontsize = Config::geti()->buttons_fontsize;
        if(Controls::geti()->controls.at(i)->name == "editor"){
            fontface = Config::geti()->editor_fontface;
            fontsize = Config::geti()->editor_fontsize;
        }
        Controls::geti()->set_font(Controls::geti()->controls.at(i)->handle, fontface, fontsize);
    }
}

void App::change_font_size(int change){
	Config::geti()->editor_fontsize += change;
	if(Config::geti()->editor_fontsize<1) Config::geti()->editor_fontsize=1;
	Controls::geti()->set_font("editor", Config::geti()->editor_fontface, Config::geti()->editor_fontsize);
    stringstream ss;
	ss<<"Rozmiar czcionki: "<<Config::geti()->editor_fontsize;
	if(Config::geti()->autoscroll_scaling){
		Config::geti()->autoscroll_interval = Config::geti()->autoscroll_interval * (Config::geti()->editor_fontsize-2*change)/(Config::geti()->editor_fontsize-change);
        Controls::geti()->set_text("autoscroll_interval", Config::geti()->autoscroll_interval);
		ss<<", Interwa³ autoscrolla: "<<Config::geti()->autoscroll_interval<<" ms";
	}
    refresh_text();
	IO::geti()->echo(ss.str());
}

void App::toolbar_switch(int change){
    if(change==1){
        Config::geti()->toolbar_show = true;
    }else if(change==0){
        Config::geti()->toolbar_show = false;
    }else if(change==2){
        Config::geti()->toolbar_show = !Config::geti()->toolbar_show;
    }
    if(Config::geti()->toolbar_show){
        for(unsigned int i=0; i<Controls::geti()->controls.size(); i++){
            Control* kontrolka = Controls::geti()->controls.at(i);
            if(kontrolka->name=="cmd") continue;
            if(string_begins(kontrolka->name, "cmd_output")) continue;
            if(kontrolka->name=="editor") continue;
            ShowWindow(kontrolka->handle, SW_SHOW);
        }
    }else{
        for(unsigned int i=0; i<Controls::geti()->controls.size(); i++){
            Control* kontrolka = Controls::geti()->controls.at(i);
            if(kontrolka->name=="cmd") continue;
            if(string_begins(kontrolka->name, "cmd_output")) continue;
            if(kontrolka->name=="editor") continue;
            ShowWindow(kontrolka->handle,SW_HIDE);
        }
    }
    event_resize();
}

void App::cmd_switch(int change){
    if(change==1){
        Config::geti()->cmd_show = true;
    }else if(change==0){
        Config::geti()->cmd_show = false;
    }else if(change==2){
        Config::geti()->cmd_show = !Config::geti()->cmd_show;
    }
    if(Config::geti()->cmd_show){
        IO::geti()->log("Ods³anianie konsoli...");
        ShowWindow(Controls::geti()->find("cmd"), SW_SHOW);
        for(int i=1; i<Config::geti()->cmd_outputs_num; i++){
            stringstream ss;
            ss<<"cmd_output"<<i+1;
            ShowWindow(Controls::geti()->find(ss.str()), SW_SHOW);
        }
    }else{
        IO::geti()->log("Ukrywanie konsoli...");
        ShowWindow(Controls::geti()->find("cmd"), SW_HIDE);
        for(int i=1; i<Config::geti()->cmd_outputs_num; i++){
            stringstream ss;
            ss<<"cmd_output"<<i+1;
            ShowWindow(Controls::geti()->find(ss.str()), SW_HIDE);
        }
    }
    event_resize();
}

void App::fullscreen_set(bool full){
	Config::geti()->fullscreen_on = full;
	if(Config::geti()->fullscreen_on){
        int window_style = GetWindowLong(main_window, GWL_STYLE);
        SetWindowLong(main_window, GWL_STYLE, window_style & ~(WS_CAPTION | WS_THICKFRAME));
        ShowWindow(main_window, SW_MAXIMIZE);
        //dla pewnoœci
		SetWindowPos(main_window, HWND_TOP, 0, 0, GetSystemMetrics(SM_CXSCREEN), GetSystemMetrics(SM_CYSCREEN), 0);
        IO::geti()->log("Fullscreen w³¹czony.");
	}else{
        int window_style = GetWindowLong(main_window, GWL_STYLE);
        SetWindowLong(main_window, GWL_STYLE, window_style | (WS_CAPTION | WS_THICKFRAME));
        ShowWindow(main_window, SW_RESTORE);
        IO::geti()->log("Fullscreen wy³¹czony.");
	}
}

void App::fullscreen_toggle(){
    fullscreen_set(!Config::geti()->fullscreen_on);
    //schowanie paska narzêdzi
    if(Config::geti()->fullscreen_on && Config::geti()->toolbar_show){
        toolbar_switch(0);
    }
}

class ProcessWindowsInfo {
public:
    DWORD ProcessID;
    vector<HWND> Windows;
    ProcessWindowsInfo(DWORD const AProcessID )
        : ProcessID( AProcessID )
    {
    }
};

BOOL __stdcall EnumProcessWindowsProc(HWND hwnd, LPARAM lParam){
    ProcessWindowsInfo *Info = reinterpret_cast<ProcessWindowsInfo*>( lParam );
    DWORD WindowProcessID;
    GetWindowThreadProcessId(hwnd, &WindowProcessID );

    char *str2 = new char[512];
	GetWindowText(hwnd, str2, 512);
    string tytul = str2;
    GetClassName(hwnd, str2, 512);
    string classname = str2;
	delete[] str2;

    if(classname=="CabinetWClass")
        cout<<"Szukam: "<<WindowProcessID<<", tytul: "<<tytul<<", klasa: "<<classname<<endl;

    if(WindowProcessID == Info->ProcessID){

        cout<<"ZNALAZL: "<<endl;

        Info->Windows.push_back(hwnd);
    }
    return true;
}

vector<HWND>* window_list(string classname){

}

void App::chordsbase_start(){
    IO::geti()->log("Otwieranie bazy akordów...");
    if(Config::geti()->songs_dir.length()==0){
        IO::geti()->error("Brak zdefiniowanej bazy akordów.");
        return;
    }
    SHELLEXECUTEINFO ShExecInfo = {0};
    ShExecInfo.cbSize = sizeof(SHELLEXECUTEINFO);
    ShExecInfo.fMask = SEE_MASK_NOCLOSEPROCESS;
    ShExecInfo.hwnd = main_window;
    ShExecInfo.lpVerb = "open";
    ShExecInfo.lpFile = "explorer.exe";
    ShExecInfo.lpParameters = Config::geti()->songs_dir.c_str();
    ShExecInfo.lpDirectory = NULL;
    ShExecInfo.nShow = SW_SHOW;
    ShExecInfo.hInstApp = NULL;
    if(!ShellExecuteEx(&ShExecInfo)){
        IO::geti()->error("B³¹d otwierania bazy akordów.");
        return;
    }
    WaitForInputIdle(ShExecInfo.hProcess, INFINITE);
    Sleep(1000);
    //WaitForSingleObject(ShExecInfo.hProcess, INFINITE);

    HANDLE process = ShExecInfo.hProcess;
    cout<<"process: "<<process<<endl;
    DWORD process_id = GetProcessId(process);
    cout<<"process_id (szukany): "<<process_id<<endl;
    ProcessWindowsInfo Info(process_id);
    EnumWindows((WNDENUMPROC)EnumProcessWindowsProc, reinterpret_cast<LPARAM>(&Info));
    // Use Info.Windows.....
    cout<<"znalezione Okna: "<<Info.Windows.size()<<endl;

    Controls::geti()->set_focus(main_window);
}

void App::quick_replace(){
    if(!Config::geti()->toolbar_show){
        toolbar_switch(1);
    }
    string selected = get_selected_text();
	selected = trim_spaces(trim_1crlf(selected));
    if(selected.length()==0){
        IO::geti()->error("Pusty tekst do zamiany.");
        return;
    }
    unsigned int sel_start, sel_end;
    get_selected_1(sel_start, sel_end);
    set_selected_1(sel_end, sel_end); //brak zaznaczenia - replace obejmuje ca³oœæ
    Controls::geti()->set_text("find_edit", selected);
    Controls::geti()->set_text("replace_edit", selected);
    Controls::geti()->select_all("replace_edit");
    Controls::geti()->set_focus("replace_edit");
}


void App::new_file(){
    Controls::geti()->set_text("editor", "");
	refresh_text();
    //usuniêcie nazwy samego pliku
    while(Config::geti()->opened_file.length()>0 && Config::geti()->opened_file[Config::geti()->opened_file.length()-1]!='\\'){
        Config::geti()->opened_file = string_cutfromend(Config::geti()->opened_file, 1);
    }
    Controls::i()->set_text("filename_edit", Config::geti()->opened_file);
	Config::geti()->opened_file = "";
	update_title();
	IO::geti()->echo("Nowy plik");
}

void App::open_chords_file(string filename){
    filename = trim_quotes(filename);
    int file_size;
	char *file_content = open_file(filename, file_size);
    if(file_content==NULL) return;
	SetWindowText(Controls::geti()->find("editor"), file_content);
    delete[] file_content;
	set_selected_1(0, 0);
	SendMessage(Controls::geti()->find("editor"), EM_SCROLLCARET, 0, 0);
	Config::geti()->opened_file = filename;
	stringstream ss;
	ss<<"Odczytano plik: "<<filename;
	IO::geti()->echo(ss.str());
    Controls::geti()->set_text("filename_edit", filename);
	refresh_text();
	update_title();
	Config::geti()->transposed = 0;
	autoscroll_off();
}

void App::save_chords_file(){
    string new_filename = Controls::geti()->get_text("filename_edit");
	if(new_filename.length()==0){
        if(!Config::geti()->toolbar_show){
            toolbar_switch(1);
        }
		IO::geti()->error("Podaj nazwê pliku!");
		return;
	}
	Config::geti()->opened_file = new_filename;
	string* str = load_text();
    if(!save_file(Config::geti()->opened_file, *str)){
        delete str;
        return;
    }
	delete str;
	update_title();
	stringstream ss;
	ss<<"Zapisano plik";
	if(Config::geti()->transposed%12 != 0){
		ss<<" (Zapis w nowej tonacji: ";
		if(Config::geti()->transposed>0) ss<<"+";
		ss<<Config::geti()->transposed;
		ss<<" !)";
	}
	IO::geti()->echo(ss.str());
}

void App::analyze(){
    int licznik = 0;
    while(skanuj_1()) licznik++;
    if(licznik==0) IO::geti()->echo("Brak zmian");
    else IO::geti()->echo("Wprowadzono zmiany");
}

void App::transpose(int transponuj){
	if(transponuj==0) return;
	Config::geti()->transposed += transponuj;
    string* str = load_text();
    string trans = transpose_string(*str, transponuj);
    delete str;
	Controls::geti()->set_text("editor", trans);
	refresh_text();
	stringstream ss;
	ss<<"Transpozycja akordów: ";
	if(Config::geti()->transposed>0) ss<<"+";
	ss<<Config::geti()->transposed;
	IO::geti()->echo(ss.str());
}
