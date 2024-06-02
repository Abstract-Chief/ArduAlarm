#include <Arduino.h>

#include "rtc_wrapper.h"
#include "sensors.h"
#include "lcd_wrapper.h"
#include "states.h"
#include "config.h"
#include"interface.h"

#include<LiquidCrystal_I2C.h>

#include <RtcDS1302.h>

extern RtcDS1302<ThreeWire> Rtc;

/**
 * Holds combination of pressed buttons.
 */
byte btn_state;


/**
 * ISR for button press.
 *
 * This function is executed, when button is pressed. The global variable
 * `btn_state` is then set according to the combination of pressed buttons.
 */
void on_button_press(){
    btn_state = 0;

    if( digitalRead(BTN1_PIN) == HIGH) {
        btn_state |= BTN1_PRESSED;
    }

    if(digitalRead(BTN2_PIN) == HIGH){
        btn_state |= BTN2_PRESSED;
    }

    if(digitalRead(BTN3_PIN) == HIGH){
        btn_state |= BTN3_PRESSED;
    }

    if(digitalRead(BTN4_PIN) == HIGH){
        btn_state |= BTN4_PRESSED;
    }
}


void setup(){
    // needed from original main() function
    init();

    pinMode(BTN1_PIN, INPUT);
    pinMode(BTN2_PIN, INPUT);
    pinMode(BTN3_PIN, INPUT);
    pinMode(BTN4_PIN, INPUT);
    pinMode(ALARM_PIN,OUTPUT);

    // init alarm clock modules
    lcd_init();
    clock_init();

    sensors_init();
}
class InitCentral : public Initer {
    void init(Interface *obj) override{
        obj->add_element(3,0,HoursType);
        obj->add_element(6,0,MinutesType);
        obj->add_element(9,0,SecondsType);
        object_add_text(obj,"<",0,0);
        object_add_text(obj,">",13,0);
        object_add_text(obj,"day:",0,1);
        obj->add_element(5,1,DayType);
        obj->add_element(8,1,MonthNameType);
    } 
};
class InitUserScreen : public Initer {
    void init(Interface *obj) override{
        // object_add_text(obj,"its you screen",0,0);
        // object_add_text(obj,"1+3 btn to add elem.",0,1);
    } 
};
class StandartMapIniter : public MapIniter {
    public:
    BudicInterface* budic;
    void init(InterfaceMap* map){
        
        TimeInterface *center=new TimeInterface(map->get_epr(),new InitCentral,true);
        map->add_interface((Interface*)center);
        budic=new BudicInterface(map->get_epr(),NULL);
        map->add_interface((Interface*)budic);
        TimeInterface *user1=new TimeInterface(map->get_epr(),new InitUserScreen,false);
        map->add_interface((Interface*)user1);
        //TimeInterface *user2=new TimeInterface(map->get_epr(),new InitUserScreen,false);
        //map->add_interface((Interface*)user2);   
    }  
};

int main() {
    setup();
    enum state state = CLOCK;
    StandartMapIniter initer;
    InterfaceMap map=InterfaceMap(0,&initer);
    map.reset();
    lcd_clear();
    while(1){
        on_button_press();
        map.use();
        initer.budic->budik_handler();
        delay(50);
    }
}
