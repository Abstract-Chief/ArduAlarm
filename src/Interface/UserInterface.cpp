#include<interface.h>
TimeInterface::TimeInterface(uint16_t epprom_start_,Initer *obj,bool deleted_) 
        : Interface(epprom_start_,obj),cursor(0),cursor2(2),caps(false),mode(0),count(0),deleted(deleted_){
    init();
}
uint16_t TimeInterface::get_mem_size()const{
    return MAX_MEMORY_SIZE;
}
void TimeInterface::extract(){
    uint8_t count_buf=EEPROM.read(epprom_start+1);
    uint8_t cur=epprom_start+2;
    for(int i=0;i<count_buf;i++){
        uint8_t x=EEPROM.read(cur++),y=EEPROM.read(cur++);
        ElementType type=(ElementType)EEPROM.read(cur++);
        add_element(x,y,type);
    }
}

void TimeInterface::save(){
    EEPROM.put(epprom_start+1,count);
    uint8_t cur=epprom_start+2;//0 - flag 1 - count 2 ... - data
    for(int i=0;i<count;i++){
        EEPROM.put(cur++,elements[i]->x);
        EEPROM.put(cur++,elements[i]->y);
        EEPROM.put(cur++,elements[i]->type);
    }
    byte flag=true;
    EEPROM.put(epprom_start,flag);
}
void TimeInterface::reset(){
    for(int i=0;i<count;i++){
        free(elements[i]);
    }
    count=0;
    EEPROM.put(epprom_start,0);
    initer->init(this);
}
bool TimeInterface::add_element(uint8_t x,uint8_t y,uint8_t type){
    if(count+1==MAX_OBJECT_COUNT) return false;
    elements[count++]=new Element(x,y,type);
    return true;
}
void TimeInterface::print(RtcDateTime *dt,int8_t cur){//cur -1 not cur
    if(count==0){
        lcd_print_at(0,0,"null interface");
        lcd_print_at(1,0,"add item ");
    }
    
    for(int i=0;i<count;i++){
        if(cur!=i)
            elements[i]->print(dt,false);
    }
    if(cur!=-1)
        elements[cur]->print(dt,true);
}
void TimeInterface::handler(RtcDateTime *dt){
    if(btn_state & BTN1_PRESSED && btn_state & BTN2_PRESSED && btn_state & BTN3_PRESSED){//all keys
        reset();
        delay(300);
        lcd_clear();
    }else if(btn_state & BTN1_PRESSED && btn_state & BTN2_PRESSED){
        mode=(mode==2 ? 0 : mode+1);
        delay(300);
        lcd_clear();
        btn_state=0;
    }else if(btn_state & BTN1_PRESSED && btn_state & BTN3_PRESSED){
        mode=(mode==3 ? 0 : 3);
        delay(300);
        lcd_clear();
        btn_state=0;
    } 
    if(mode==0){
        print(dt,-1);
        lcd_print_at(1,15,"T");
    }else if(mode==1){
        change(dt);
        lcd_print_at(1,15,"E");
    }else if(mode==2){
        construct(dt);
        lcd_print_at(1,15,"C");
    }else if(mode==3){
        adder();
        lcd_print_at(1,15,"A");
    }
    
}

void TimeInterface::change(RtcDateTime *dt){
    if(count==0) {mode=0;return;}
    print(dt,cursor);
    if(btn_state & BTN1_PRESSED){
        next(true);
        delay(100);
        lcd_clear();
    }
    if(btn_state & BTN3_PRESSED){
        if(elements[cursor]->type>AdvensedTypes) return;
        byte *obj=((byte*)dt+elements[cursor]->type);
        *obj=(elements[cursor]->max_value(dt)==*obj+1 ? 0 : *obj+1);
        Rtc.SetDateTime(*dt);
        delay(100);
    }
    if(btn_state & BTN2_PRESSED){
        if(elements[cursor]->type>AdvensedTypes) return;
        byte *obj=((byte*)dt+elements[cursor]->type);
        *obj=(0==*obj ? elements[cursor]->max_value(dt)-1 : *obj-1);
        Rtc.SetDateTime(*dt);
        delay(100);
    }
    if(btn_state==BTN4_PRESSED){
        mode=0;
        delay(300);
        lcd_clear();
    }
}
void TimeInterface::next(bool type){
    do{
        cursor=(cursor==count-1 ? 0 : cursor+1);
        if(!type) break;
    }while(elements[cursor]->type>GrafickTypes);
}
void TimeInterface::construct(RtcDateTime *dt){
    if(count==0) mode=0;
    print(dt,cursor);
    if(btn_state & BTN3_PRESSED && btn_state & BTN4_PRESSED){
        del_element(cursor);
        save();
        delay(100);
        lcd_clear();
    }
    else if(btn_state & BTN1_PRESSED){
        next(false);
        save();
        delay(100);
        lcd_clear();
    }else if(btn_state & BTN2_PRESSED){
        elements[cursor]->x-=(elements[cursor]->x==0 ? 0 : 1);
        save();
        delay(100);
        lcd_clear();
    }else if(btn_state & BTN3_PRESSED){
        elements[cursor]->move(1,0);
        save();
        delay(100);
        lcd_clear();
    }else if(btn_state & BTN4_PRESSED){
        elements[cursor]->y=!elements[cursor]->y;
        save();
        delay(100);
        lcd_clear();
    } 
}
bool TimeInterface::can_move()const{
    //lcd_clear();
    //lcd_printf_at(0,0,"can_move %d",mode);
    return mode==0;
}
char *adder_dict_basic[]={"Year","Month","Day","Hours","Min","Sec"};
char *adder_dict_extra[]={"Y2000","Temp","Hydr","Week","Month"};
char myascii[]="ABCDEFGHIJKLMNOPQRSTUVWXYZ1234567890:/()$#@!^.,'\"";
bool TimeInterface::can_delete()const{
    return deleted;
}
void TimeInterface::adder(){
    if(cursor>=3) cursor=0;
    lcd_printf_at(0,0,"%sbasic",(cursor==0 ? ">" : " "));
    lcd_printf_at(0,7,"%sextra",(cursor==1 ? ">" : " "));
    lcd_printf_at(1,0,"%sascii",(cursor==2 ? ">" : " "));
    lcd_printf_at(0,15,"%c",(caps ? '^' : ' '));

    uint8_t max_modes[3]={sizeof(adder_dict_basic)/sizeof(char*),sizeof(adder_dict_extra)/sizeof(char*),sizeof(myascii)-1};//
    uint8_t max_cur_mode=max_modes[cursor];
    if(cursor2>=max_cur_mode) cursor2=0;

    if(cursor==0)
        lcd_printf_at(1,7,"+ %s  ",adder_dict_basic[cursor2]);
    else if(cursor==1)
        lcd_printf_at(1,7,"+ %s  ",adder_dict_extra[cursor2]);
    else{
        uint8_t prev=(cursor2==0 ? ' ' : (caps ? myascii[cursor2-1] : tolower(myascii[cursor2-1])));
        uint8_t cur=(caps ? myascii[cursor2] : tolower(myascii[cursor2]));
        uint8_t n=(cursor2==max_cur_mode-1 ? ' ' : (caps ? myascii[cursor2+1] : tolower(myascii[cursor2+1])));
        lcd_printf_at(1,6,"%c(%c %c %c)  ",(caps ? '+' : '-'),prev,cur,n);
    }
    if(btn_state & BTN4_PRESSED){
        uint8_t type=cursor2;
        if(cursor==1) type=AdvensedTypes+1+cursor2;
        else if(cursor==2){
            type=(caps ? myascii[cursor2] : tolower(myascii[cursor2]));
        }
        if(add_element(0,0,type)==true){
            lcd_clear();
            cursor=count-1;
            mode=2;
            save();
        }else{
            mode=2;
            lcd_clear();
            lcd_print_at(0,0,"Overload");
            lcd_print_at(1,0,"delete obj");
            delay(2000);
            lcd_clear();
        }
    }
    if(btn_state & BTN2_PRESSED && btn_state & BTN3_PRESSED){
        caps=!caps;
        delay(100);
    }
    else if(btn_state & BTN1_PRESSED){
        cursor=(cursor==2 ? 0 : cursor+1);
        delay(100);
    }else if(btn_state & BTN2_PRESSED){
        cursor2=(cursor2==0 ? max_cur_mode-1 : cursor2-1);
        delay(100);
    }else if(btn_state & BTN3_PRESSED){
        cursor2=(cursor2==max_cur_mode ? 0 : cursor2+1);
        delay(100);
    }
}
 void TimeInterface::del_element(uint8_t n){
    cursor=0;
    if(count==0) return;
    if(n>=count) return;
    delete elements[n];
    count--;
    if(count==0) cursor=-1; 
    for(int i=n;i<count;i++) elements[i]=elements[i+1];
 }

char *MonthName_dict[12]={"Jan","Feb","Mar","Apr","May","Jun","Jul","Aug","Sep","Oct","Nov","Dec"};
char *WeekDay_dict[7]={"Sun","Mon","Tue","Wed","Thu","Fri","Sat"};
Element::Element(uint8_t x_, uint8_t y_,uint8_t type_) : x(x_),y(y_),type(type_){}
void Element::print(const RtcDateTime* dt,bool active){
    byte *obj=((byte*)dt+type);
    last=*((byte*)dt+type);
    if(type<AdvensedTypes){
        lcd_printf_at(y,x,"%s%d%s",(active ? ">" : (*obj<10 ? SPACE_TIME_SYMBOL : "")),(type==YearType)*2000+*obj,(active ? "" : ""));
        return;
    }
    if(type<GrafickTypes){
        switch (type)
        {
        case YearFrom2000Type:
            lcd_printf_at(y,x,"%s%d%s",(active ? ">" : ""),*((byte*)dt+YearType),(active ? "" : ""));
            break;
        case TemperatureType:
            lcd_printf_at(y,x,"%s%.1fC%s",(active ? ">" : ""),get_temperature(),(active ? "" : ""));
            break;
        case HydroType:
            lcd_printf_at(y,x,"%s%dH%s",(active ? ">" : ""),get_humidity(),(active ? "" : ""));
            break;
        case WeekdayType:
            lcd_printf_at(y,x,"%s%s%s",(active ? ">" : ""),WeekDay_dict[dt->DayOfWeek()],(active ? "" : ""));
            break;
        case MonthNameType:
            lcd_printf_at(y,x,"%s%s%s",(active ? ">" : ""),MonthName_dict[dt->Month()-1],(active ? "" : ""));
            break;
        default: break;
        }
        return;
    }
    if(active) lcd_print_at(y,x,"_");
    else lcd_printf_at(y,x,"%c",type);

}
void Element::move(uint8_t dx,uint8_t dy){
    if(x+dx<16 && y+dy<2 && x+dx>=0 && y+dy>=0){
        x+=dx;
        y+=dy;
    }  
}
void Element::set(uint8_t x_, uint8_t y_){
    if(x_<16 && y_<2){
        x=x_;
        y=y_;
    }
}

uint8_t Element::max_size(){
    switch (type){
    case YearType: return 4;
    default: return 2;
    }
}
bool isLeapYear(int year){
  if ((year % 4 == 0 && year % 100 != 0) || (year % 400 == 0)) {
    return true;
  }
  return false;
}
uint8_t max_month_day[]= {31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31 };
int8_t Element::max_value(RtcDateTime *dt){
    switch (type){
        case MonthType: return 12;
        case DayType: return (isLeapYear(dt->Year()) && dt->Month()==1 ? 29 : max_month_day[dt->Month()]);
        case HoursType: return 24;
        case MinutesType: return 60;
        case SecondsType: return 60;
        default: return -1;
    }
    return -1;
}