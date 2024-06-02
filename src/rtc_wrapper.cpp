#include<rtc_wrapper.h>
#include<lcd_wrapper.h>
#include<interface.h>
#include <RtcDS1302.h>

ThreeWire myWire(3,4,2); // IO, SCLK, CE
RtcDS1302<ThreeWire> Rtc(myWire);
// uint8_t _yearFrom2000;
//     uint8_t _month;
//     uint8_t _dayOfMonth;
//     uint8_t _hour;
//     uint8_t _minute;
//     uint8_t _second;

void clock_init(){
    lcd_clear();
    Rtc.Begin();
    
}
void set_datetime(const byte day, const byte month, const int year, const byte hours, const byte minutes, const byte seconds){
    RtcDateTime compiled = RtcDateTime(year,month,day,hours,minutes,seconds);
    if (compiled.IsValid()) 
    {
        Serial.println("RTC lost confidence in the DateTime!");
        lcd_clear();
        lcd_print_at(0,0,"update time");
        delay(700);
        lcd_clear();
        Rtc.SetDateTime(compiled);
    }
}
byte get_day(){
    return Rtc.GetDateTime().Day();
}


/**
 * Gets month from RTC
 */
byte get_month(){return Rtc.GetDateTime().Month();}


/**
 * Gets year from RTC
 */
int get_year(){return Rtc.GetDateTime().Year();}


/**
 * Gets hour from RTC
 *
 * @return byte
 */
byte get_hours(){return Rtc.GetDateTime().Hour();}


/**
 * Gets minute from RTC
 */
byte get_minutes(){return Rtc.GetDateTime().Minute();}


/**
 * Gets second from RTC
 */
byte get_seconds(){return Rtc.GetDateTime().Second();}
 
/**
 * @brief
 *
 * @return struct dt
 */
struct dt now();