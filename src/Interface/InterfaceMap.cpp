#include<interface.h>

#define MAP_BUFFER_SIZE 2



InterfaceMap::InterfaceMap(uint16_t epr_start_,MapIniter *initer_) : count(0),current(-1),epr_start(epr_start_),initer(initer_){
    // if(EEPROM.read(epr_start)==true){
    //     extract();
    // }else initer->init(this);
    initer->init(this);
}
InterfaceMap::~InterfaceMap(){
    for(int i=count-1;i>0;i++)
        delete interfaces[i];
    count=0;
}
void InterfaceMap::save(){
    // if(count<=2) return;
    // EEPROM.put(epr_start,true);
    // EEPROM.put(epr_start+1,count);
}
void InterfaceMap::extract(){
    // initer->init(this);
    // uint8_t buf_count=EEPROM.read(epr_start+1);
    // for(int i=0;i<buf_count-2;i++){
    //     TimeInterface* temp=new TimeInterface(get_epr(),new InitUserScreen,true);
    //     add_interface((Interface*)temp);
    // }
}
void InterfaceMap::add_interface(Interface *obj){
    if(current==MAX_INTERFACE_COUNT-1) return;
    if(current==-1) current=0;
    interfaces[count]=obj;
    count++;
}
void InterfaceMap::del_interface(uint8_t n){
    if(count==0 || n>=count) return;
    if(current==n) current-=1;
    delete interfaces[n];
    EEPROM.put(interfaces[n]->epprom_start,false);
    uint8_t buf_count=count;
    count=0;
    for(int i=0;i<buf_count;i++){
        if(i==n) continue;
        interfaces[i]->epprom_start=get_epr();
        interfaces[i]->save();
        add_interface(interfaces[i]);
    }
    save();
}
uint8_t InterfaceMap::get_current() const{
    return current;
}
void InterfaceMap::reset(){
    EEPROM.put(epr_start,true);
}
void InterfaceMap::handler(){
    // if(btn_state &BTN1_PRESSED && btn_state &BTN2_PRESSED && btn_state & BTN3_PRESSED){
    //     if(interfaces[current]->can_delete()){
    //         del_interface(current);
    //         current=0;
    //         delay(300);
    //     }
    // }
    // if(btn_state &BTN2_PRESSED && btn_state & BTN3_PRESSED && btn_state & BTN4_PRESSED){
    //     add_interface((Interface*)(new TimeInterface(get_epr(),new InitUserScreen(),true)));
    //     current=count-1;
    //     delay(300);
    //     save();
    // } vela bagov
    
    if(btn_state==BTN1_PRESSED){
        current=(current==0 ? count-1 : current-1);
        delay(200);
        lcd_clear();
    }else if(btn_state==BTN4_PRESSED){
        current=(current==count-1 ? 0 : current+1);
        delay(200);
        lcd_clear();
    }
    lcd_printf_at(0,15,"%d",current);
}
uint16_t InterfaceMap::get_epr(){
    uint16_t cur=epr_start+MAP_BUFFER_SIZE;
    for(int i=0;i<count;i++){
        cur+=interfaces[i]->get_mem_size()+2;
    }
    return cur;
}
void InterfaceMap::use(){
    if(current==-1) return;
    if(interfaces[current]->can_move()) handler();
    RtcDateTime dt=Rtc.GetDateTime();
    interfaces[current]->handler(&dt);
}