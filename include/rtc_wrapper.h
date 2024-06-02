#ifndef _CLOCK_H
#define _CLOCK_H

#include <Arduino.h>


struct dt {
    byte day;
    byte month;
    int year;
    byte hours;
    byte minutes;
    byte seconds;
};


/**
 * Initialize clock (RTC) module
 *
 */
void clock_init();


/**
 * Sets date on RTC
 */
void set_date(const byte day, const byte month, const int year);


/**
 * Sets time on RTC
 *
 * @param hours
 * @param minutes
 * @param seconds
 */
void set_time(const byte hours, const byte minutes, const byte seconds);


/**
 * Sets date and time on RTC
 *
 * @param day
 * @param month
 * @param year
 * @param hours
 * @param minutes
 * @param seconds
 */
void set_datetime(const byte day, const byte month, const int year, const byte hours, const byte minutes, const byte seconds);


/**
 * Gets day from RTC
 */
byte get_day();


/**
 * Gets month from RTC
 */
byte get_month();


/**
 * Gets year from RTC
 */
int get_year();


/**
 * Gets hour from RTC
 *
 * @return byte
 */
byte get_hours();


/**
 * Gets minute from RTC
 */
byte get_minutes();


/**
 * Gets second from RTC
 */
byte get_seconds();


/**
 * @brief
 *
 * @return struct dt
 */
struct dt now();

#endif
