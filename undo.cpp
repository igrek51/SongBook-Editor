#include "app.h"

UndoHistory::UndoHistory(string *str){
    scroll = App::geti()->get_scroll();
    App::geti()->get_selected_1(sel_start, sel_end);
    content = *str;
}

void Undo::revert(){
    if(undos.size()==0){
        IO::geti()->error("Brak zapisanej historii pliku do przywrócenia.");
        return;
    }
    //przywrócenie poprzedniej wersji
    Controls::geti()->set_text("editor", undos.at(0)->content);
	App::geti()->format_text(&undos.at(0)->content);
	App::geti()->set_scroll(undos.at(0)->scroll);
    App::geti()->set_selected_1(undos.at(0)->sel_start, undos.at(0)->sel_end);
    //usuniêcie z historii przywróconego elementu
    delete undos.at(0);
    undos.erase(undos.begin());
    IO::geti()->echo("Cofniêto zmiany do poprzedzniej wersji pliku.");
}

void Undo::save(string *str){
    //zapisanie kopii aktualnego stanu
    undos.insert(undos.begin(), new UndoHistory(str));
    //obciêcie historii do danej liczby
    if(Config::geti()->undos_number > 0){
        while((int)undos.size() > Config::geti()->undos_number){
            delete undos.back();
            undos.erase(undos.begin() + undos.size()-1); //usuwanie ostatniego
        }
    }
}

void Undo::save(){
    string* str = App::geti()->load_text();
    save(str);
    delete str;
}

void Undo::reset(){
    for(unsigned int i=0; i<undos.size(); i++){
        delete undos.at(i);
    }
    undos.clear();
}
