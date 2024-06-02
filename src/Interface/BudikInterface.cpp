#include<interface.h>
#include<music.h>
void on_button_press();
extern byte btn_state;

bool play_music(MUSIC_TYPE *music,int temp,int size){
    int notes = size / 2;
    int wholenote = (60000 * 4) / temp;
    int divider = 0, noteDuration = 0;
    for (int thisNote = 0; thisNote < notes * 2; thisNote = thisNote + 2) {
        divider = music[thisNote + 1];
        on_button_press();
        if(btn_state!=0) return true;
        if (divider > 0) {
            noteDuration = (wholenote) / divider;
        } else if (divider < 0) {
            noteDuration = (wholenote) / abs(divider);
            noteDuration *= 1.5;
        }
        tone(ALARM_PIN, music[thisNote], noteDuration * 0.9);
        delay(noteDuration);
        noTone(ALARM_PIN);
    }
    return false;
}
bool music_handler(enum Musics type){
    switch (type)
    {
        case StarWars:return play_music(StarWars_Music,StarWars_Temp,sizeof(StarWars_Music));
        //case GameOfTrons:return play_music(GameOfTrons_Music,GameOfTrons_Temp,sizeof(GameOfTrons_Music));
        case HappyBirthday:return play_music(HappyBirthday_Music,HappyBirthday_Temp,sizeof(HappyBirthday_Music));
    }
}

BudicElement::BudicElement(uint16_t time_,uint8_t music_type_) : time(time_),music_type(music_type_){}
void BudicElement::extract(uint16_t cur){
    uint8_t t_low=EEPROM.read(cur),t_high=EEPROM.read(cur+1);
    time=(t_high<<8) | t_low;
    music_type=EEPROM.read(cur+2);
}
void BudicElement::save(uint16_t cur){
    EEPROM.write(cur,time & 0xFF);
    EEPROM.write(cur + 1, (time >> 8) & 0xFF);
    EEPROM.write(cur+2,music_type);
}

void BudicElement::alarm(){
    lcd_clear();
    lcd_printf_at(0,0,"Wake Up");
    lcd_printf_at(1,0,"Its you time");
    while(btn_state==0){
        if(music_handler((enum Musics)music_type)) break;
    }
    lcd_clear();
    delay(1000);

}
void BudicElement::set_time(uint16_t time_){time=time_;}
void BudicElement::set_music(uint8_t music){music_type=music;}
uint16_t BudicElement::get_time()const{return time;}

bool BudicInterface::can_move()const{
    return mode==0;
}
void BudicInterface::extract(){
    if(EEPROM.read(epprom_start)==false) return;
    uint8_t count_buf=EEPROM.read(epprom_start+1);
    uint8_t cur=epprom_start+2;
    for(int i=0;i<count_buf;i++){
        // uint8_t t_low=EEPROM.read(cur),t_high=EEPROM.read(cur+1);
        // uint16_t time=(t_high<<8) | t_low;
        times[i].extract(cur);
        cur+=sizeof(times[i]);
        count_times++;
    }

}
void BudicInterface::save(){
    EEPROM.put(epprom_start+1,count_times);
    uint8_t cur=epprom_start+2;//0 - flag 1 - count 2 ... - data
    for(int i=0;i<count_times;i++){
        times[i].save(cur);
        cur+=sizeof(times[i]);
        
    }
    byte flag=true;
    EEPROM.put(epprom_start,flag);
}
void BudicInterface::reset(){
    count_times=0;
    save();
}
BudicInterface::BudicInterface(uint16_t epprom_start_,Initer *obj)
 : Interface(epprom_start_,obj), mode(0),count_times(0),cursor(0),cursor2(0),cursor3(0) {
    init();
}
int8_t BudicInterface::get_min_alarm(RtcDateTime *dt){
    uint16_t cur_t=dt->Minute()+dt->Hour()*60;
    uint16_t min=5000;
    int8_t min_index=-1;
    for(uint8_t i=0;i<count_times;i++){
        int16_t diff_time=times[i].get_time()-cur_t;
        if(diff_time<1) diff_time+=24*60; 
        if(min>diff_time){
            min=diff_time;
            min_index=i;
        }
    }
    return min_index;
}


#define PRINT_TIME(Y,X,H,M) lcd_printf_at(Y,X,"%s%d:%s%d",(H<10 ? SPACE_TIME_SYMBOL : ""),H,(M<10 ? SPACE_TIME_SYMBOL : ""),M)
#define PRINT_TIME_M(Y,X,Minute) PRINT_TIME(Y,X,(int)(Minute/60),(int)(Minute%60))
void BudicInterface::print(RtcDateTime *dt){
    int8_t min=get_min_alarm(dt);
    int16_t diff_time=times[min].get_time()-(dt->Minute()+dt->Hour()*60);
    lcd_printf_at(0,0,"nearest> ");
    lcd_print_at(1,15,"B");
    if(min!=-1){
        uint8_t m=times[min].get_time()%60,h=times[min].get_time()/60;
        PRINT_TIME(0,8,h,m);
        lcd_printf_at(0,13,"%s",(diff_time<0 ? "N" : " "));
    }else lcd_print_at(0,9,"NULL ");
    if(count_times==0) return;
    if(cursor>=count_times) cursor=0;
    if(cursor!=0){
        lcd_print_at(1,4,"<");
        uint8_t h=times[cursor-1].get_time()/60;
        lcd_printf_at(1,2,"%s%d",(h<10 ? SPACE_TIME_SYMBOL : ""),h);
    }else lcd_print_at(1,2,"   ");
    if(cursor!=count_times-1){
        lcd_print_at(1,10,">");
        uint8_t h=times[cursor+1].get_time()/60;
        lcd_printf_at(1,11,"%s%d",(h<10 ? SPACE_TIME_SYMBOL : ""),h);
    }else lcd_print_at(1,10,"   ");
    PRINT_TIME_M(1,5,times[cursor].get_time());

}
void BudicInterface::create_buffer_time(RtcDateTime *dt){
    buffer_time.set_time(dt->Hour()*60+dt->Minute());
    buffer_time.set_music(0);
}
void BudicInterface::create_buffer_time(uint16_t time){
    buffer_time.set_time(time);
    buffer_time.set_music(0);
    
}

void BudicInterface::budik_handler(){
    RtcDateTime dt=Rtc.GetDateTime();
    uint16_t cur_t=dt.Minute()+dt.Hour()*60;
    for(uint8_t i=0;i<count_times;i++)
        if(times[i].get_time()==cur_t){ 
            times[i].alarm();
            del_time(i);
            break;
        }
}
void BudicInterface::handler(RtcDateTime *dt){
    //budik_handler(dt);
    if(btn_state & BTN1_PRESSED && btn_state & BTN2_PRESSED && btn_state & BTN3_PRESSED){
        lcd_clear();
        reset();
        delay(100);
        return;
    }
    if(btn_state & BTN1_PRESSED && btn_state & BTN2_PRESSED){
        mode=(mode==2 ? 0 : mode+1);
        if(mode==2) create_buffer_time(dt);
        if(mode==1){
            cursor3=times[cursor2].get_music();
            save();
        }
        lcd_clear();
        delay(200);
        return;
    }
    
    if(mode==0){
        if(btn_state == BTN2_PRESSED){
            cursor=(cursor==0 ? count_times-1 : cursor-1);
            delay(100);
        }
        if(btn_state == BTN3_PRESSED){
            cursor=(cursor==count_times-1 ? 0 : cursor+1);
            delay(100);
        }
        if(btn_state & BTN3_PRESSED && btn_state & BTN4_PRESSED){
            del_time(cursor);
            delay(700);
            lcd_clear();
            save();
        }
        print(dt);
    }else if(mode==1){
        changer(dt);
    }else if(mode==2){
        adder(dt);
    }
}
bool BudicInterface::can_delete()const{
        return false;
}
bool BudicInterface::add_time(){
    return add_time(buffer_time);
}
bool BudicInterface::add_time(BudicElement el){
    if(count_times+1==MAX_BUDIK_TIMES) return false;
    times[count_times]=el;
    count_times++;
    save();
    return true;
}
void BudicInterface::del_time(uint8_t n){
    if(count_times==0) return;
    for(int i=n;i<count_times-1;i++){
        times[i]=times[i+1];
    }
    count_times--;
    cursor-=(cursor!=-1);
}
void BudicInterface::changer(RtcDateTime *dt){
    if(count_times==0){
        mode=2;
        create_buffer_time(dt);
        return;
    }
    ac_handler(&times[cursor],dt);
    lcd_print_at(1,15,"C");
    if(btn_state == BTN4_PRESSED){
        save();
        mode=0;
        lcd_clear();
        delay(100);
        lcd_clear();
    }
    
}
char *MelodyTypesName[3]={"StarWars","birthday"};
void  BudicInterface::ac_handler(BudicElement* time_c,RtcDateTime *dt){
    if(btn_state == BTN1_PRESSED){
        cursor2=(cursor2==2 ? 0 : cursor2+1);
        delay(100);
    }else if(btn_state == BTN2_PRESSED){
        uint8_t h=time_c->get_time()/60,m=time_c->get_time()%60;
        if(cursor2<2){
            if(cursor2) time_c->set_time(h*60+(m==0 ? 59 : m-1));
            else time_c->set_time((h==0 ? 23 : h-1)*60+m);
            delay(50);
        }
        else{
            cursor3=(cursor3==1 ? 0 : cursor3+1);
            delay(50);
            lcd_clear();
        }
    }else if(btn_state == BTN3_PRESSED){
        uint8_t h=time_c->get_time()/60,m=time_c->get_time()%60;
        if(cursor2<2){
            if(cursor2) time_c->set_time(h*60+(m+1==60 ? 0 : m+1));
            else time_c->set_time((h+1==24 ? 0 : h+1)*60+m);
            delay(50);
        }
        else{
            cursor3=(cursor3==1 ? 0 : cursor3+1);
            time_c->set_music(cursor3);
            delay(50);
            lcd_clear();
        }
    }
    uint8_t h=time_c->get_time()/60,m=time_c->get_time()%60;
    lcd_print_at(0,0,"time:");
    lcd_printf_at(1,0,"%smus:%s",(cursor2==2 ? ">" : ""),MelodyTypesName[cursor3]);
    PRINT_TIME(0,6,h,m);
    //lcd_printf_at(1,6,"%s %s",(cursor2 ? "  " : "^^"),(cursor2 ? "^^" : "  "));
    if(cursor2<2)
        lcd_printf_at(0,12,"<%c",(cursor2 ? 'M' : 'H'));
    else
        lcd_print_at(0,12,"  ");
    

}
void BudicInterface::adder(RtcDateTime *dt){
    lcd_print_at(1,15,"A");
    ac_handler(&buffer_time,dt);
    if(btn_state & BTN4_PRESSED){
        mode=0;
        lcd_clear();
        save();
        if(!add_time()){
            lcd_print_at(0,0,"overload");
            lcd_printf_at(1,0,"max points is %d",MAX_BUDIK_TIMES);
            delay(1000);
            lcd_clear();
        }
        delay(100);
        return;
    }

    
}
uint16_t BudicInterface::get_mem_size()const{
    return sizeof(times);    
}