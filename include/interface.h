#ifndef INTERFACE_H_SENTURY
#define INTERFACE_H_SENTURY
#include <RtcDS1302.h>
#include<EEPROM.h>

#include<lcd_wrapper.h>
#include<interface.h>
#include<config.h>
#include<rtc_wrapper.h>
#include<sensors.h>

extern byte btn_state;
extern RtcDS1302<ThreeWire> Rtc;

#define SPACE_TIME_SYMBOL "0"

#define MAX_OBJECT_COUNT 30
#define MAX_INTERFACE_COUNT 4
#define MAX_BUDIK_TIMES 32

#define MAX_MEMORY_SIZE (MAX_OBJECT_COUNT*3+2)



enum ElementType{
    YearType=0,
    MonthType,
    DayType,
    HoursType,
    MinutesType,
    SecondsType,
    //advenced
    AdvensedTypes,
    YearFrom2000Type,
    TemperatureType,
    HydroType,
    WeekdayType,
    MonthNameType,
    //grafick
    GrafickTypes,


};

class TimeInterface;
class Initer;
class InterfaceMap;

class Element{
    uint8_t x,y,last;
    uint8_t type;
    friend TimeInterface;
    public:
    Element(uint8_t x_, uint8_t y_,uint8_t type_);
    uint8_t max_size();
    int8_t max_value(RtcDateTime *dt);
    void print(const RtcDateTime* dt,bool active=false);
    void move(uint8_t dx,uint8_t dy);
    void set(uint8_t x_, uint8_t y_);
};

class Interface{
    friend InterfaceMap;

    protected:
    uint16_t epprom_start;
    Initer *initer;
    void init();
    public:
    Interface(uint16_t epr,Initer *initer_);
    virtual void extract() = 0;
    virtual void save() = 0;
    virtual bool can_delete()const=0;
    virtual bool can_move()const = 0;
    virtual bool add_element(uint8_t x,uint8_t y,uint8_t type);
    virtual void handler(RtcDateTime *dt) = 0;
    virtual uint16_t get_mem_size()const = 0;
};

class Initer{
    public:
    virtual void init(Interface *obj)=0;
};

class TimeInterface : Interface{

    protected:
    int8_t cursor,cursor2;
    bool caps;
    Element *elements[MAX_OBJECT_COUNT];
    uint8_t mode;
    uint8_t count;
    bool deleted;
    private:
    void change(RtcDateTime *dt);
    void construct(RtcDateTime *dt);
    void adder();
    void next(bool type);
    void reset();
    protected:
    void del_element(uint8_t n);
    void print(RtcDateTime *dt,int8_t cur);
    public:
    TimeInterface(uint16_t epr,Initer *initer_,bool deleted);
    virtual bool can_move()const;
    bool can_delete()const;
    void extract() override;
    void save() override;
    bool add_element(uint8_t x,uint8_t y,uint8_t type) override;
    
    void handler(RtcDateTime *dt) override;

    uint16_t get_mem_size()const override;
};
class BudicElement{
    uint16_t time;
    uint8_t music_type;
    public:
    BudicElement(uint16_t time_,uint8_t music_type_);
    BudicElement(){}
    void extract(uint16_t cur);
    void save(uint16_t cur);
    void alarm();
    void set_time(uint16_t time_);
    void set_music(uint8_t music);
    uint16_t get_time()const;
    uint8_t get_music()const{return music_type;}
    
};
class BudicInterface : public Interface{
    uint8_t mode;
    BudicElement times[MAX_BUDIK_TIMES];
    uint8_t count_times;
    BudicElement buffer_time;
    uint8_t cursor,cursor2,cursor3;

    BudicElement budic_minute;
    protected:
    void extract() override;
    void save() override; 
    bool add_time();
    bool add_time(BudicElement minutes);
    void del_time(uint8_t n);
    void reset();
    void print(RtcDateTime *dt);

    

    void changer(RtcDateTime *dt);
    void adder(RtcDateTime *dt);
    void ac_handler(BudicElement* el,RtcDateTime *dt);
    void menu(RtcDateTime *dt);
    void create_buffer_time(RtcDateTime *dt);
    void create_buffer_time(uint16_t time);
    int8_t get_min_alarm(RtcDateTime *dt);
    

    public:

    BudicInterface(uint16_t epprom_start_,Initer *obj);
    bool can_move()const;
    bool can_delete()const;
    void handler(RtcDateTime *dt) override;
    void budik_handler();
    uint16_t get_mem_size()const override;
};
class MapIniter{
    public:
    virtual void init(InterfaceMap* map)=0;
};
class InterfaceMap{
    Interface *interfaces[MAX_INTERFACE_COUNT];
    uint8_t count;
    int8_t current;
    uint16_t epr_start;
    MapIniter *initer;
    protected:
    void save();
    void extract();
    public:
    InterfaceMap(uint16_t epr_start_,MapIniter *intiter);
    ~InterfaceMap();
    void handler();
    void add_interface(Interface *obj);
    void del_interface(uint8_t n);
    uint16_t get_epr();
    uint8_t get_current() const;
    void use();
    void reset();

};
void object_add_text(Interface *obj,char *text,int x,int y);
#endif