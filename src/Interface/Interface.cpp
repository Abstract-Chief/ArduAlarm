#include<interface.h>
void object_add_text(Interface *obj,char *text,int x,int y){
    for(int i=0;text[i];i++){
        obj->add_element(x+i,y,text[i]);
    }
}

Interface::Interface(uint16_t epprom_start_,Initer *obj) : epprom_start(epprom_start_),initer(obj){}
bool Interface::add_element(uint8_t x,uint8_t y,uint8_t type){
    return 0;
}
void Interface::init(){
    if(EEPROM.read(epprom_start)==true) extract();
    else{
        if(initer!=NULL)
            initer->init(this);
    }
}
