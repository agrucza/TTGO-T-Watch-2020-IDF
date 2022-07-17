#pragma once

#include <i2c_helper.h>
#include <pcf8563.h>

class RTC
{
    const char          *TAG        = "RTC";
    i2c_port_t          i2c_port    = I2C_NUM_0;
    pcf8563_t           pcf;
    char                buffer[128];

public:
    RTC(void){}
    void init();
    pcf8563_t getPCF(){ return pcf; }
    char* getRTCTime(char* format);
    void setRTCTime(int year, int month, int day, int hour, int minute, int second);
    void setRTCAlarm( int hour, int minute, int dayOfWeek, int dayOfMonth);
    tm getRTCAlarm();
    void setTimer(uint8_t seconds);
};
