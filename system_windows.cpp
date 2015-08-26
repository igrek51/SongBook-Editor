#include "system.h"

System* System::instance = NULL;

System* System::geti(){
    if(instance == NULL){
        instance = new System();
    }
    return instance;
}
