/**
 * @file RTC.cpp
 * @author agrucza (a.grucza@gmx.net)
 * @brief This class will provide access to the PCF8563 chip
 * @version 0.1
 * @date 2022-07-18
 * 
 * @copyright Copyright (c) 2022
 * 
 */

#include "RTC.hpp"

#include <time.h>
#include <esp_log.h>
#include <i2c_helper.h>
#include <pcf8563.h>

/**
 * @brief setup RTC I2C connection and init chip
 * 
 */
void RTC::init()
{
    ESP_LOGI(TAG, "Initializing PCF8563");
    pcf.read = &i2c_read;
    pcf.write = &i2c_write;
    pcf.handle = &i2c_port;

    pcf8563_init(&pcf);

    //setRTCTime(2022, 7, 17, 16, 6, 50);

    getRTCTime("%c (doy %j and dow %w)");
}

/**
 * @brief get RTC date and time with strftime formated string
 * 
 * @param format strftime format
 * @return char* date and time based on format
 */
char* RTC::getRTCTime(char* format)
{
    tm rtc_time = {};

    pcf8563_read(&pcf, &rtc_time);

    strftime(buffer, 128 ,format, &rtc_time);
    ESP_LOGI(TAG, "%s", buffer);

    return buffer;
}

/**
 * @brief Set the RTC to the values handed over as attributes
 * Year and month have to be recalculated according to the PCF8563 datasheet.
 * PCF8563 only accepts the following formarts:
 *      year: actual year - 1900 -> 2022 - 1900 = 122
 *      month: actual month -1 (month will start with zero)
 * 
 * @param year 
 * @param month 
 * @param day 
 * @param hour 
 * @param minute 
 * @param second 
 */
void RTC::setRTCTime(int year, int month, int day, int hour, int minute, int second)
{
    tm new_time = {};

    new_time.tm_year = (year - 1900);
    new_time.tm_mon  = (month - 1);
    new_time.tm_mday = day;
    new_time.tm_hour = hour;
    new_time.tm_min  = minute;
    new_time.tm_sec  = second;
    
    pcf8563_write(&pcf, &new_time);
}

/**
 * @brief Will set an alarm to the RTC chip according to the attributes
 * Hour and minute is self explaining i guess.
 * If no dayOfWeek or dayOfMonth is set the alarm will be set for each day.
 * If set the alarm will be for this day of week or day of month.
 * 
 * @param hour 
 * @param minute 
 * @param dayOfWeek 
 * @param dayOfMonth 
 */
void RTC::setRTCAlarm(int hour, int minute, int dayOfWeek = PCF8563_ALARM_NONE, int dayOfMonth = PCF8563_ALARM_NONE)
{
    // tmp will be needed to check for triggered alarm
    //uint8_t tmp;
    tm rtc_alarm = {};

    rtc_alarm.tm_hour   = hour;
    rtc_alarm.tm_min    = minute;
    rtc_alarm.tm_wday   = dayOfWeek;
    rtc_alarm.tm_mday   = dayOfMonth;

    pcf8563_ioctl(&pcf, PCF8563_ALARM_SET, &rtc_alarm);

    /* Later check if alarm is triggered. */
    // pcf8563_ioctl(&pcf, PCF8563_CONTROL_STATUS2_READ, &tmp);
    // if (tmp & PCF8563_AF) {
    //     printf("Got alarm!");
    // };

    /* And clear the alarm flag. */
    // tmp &= ~PCF8563_AF;
    // pcf8563_ioctl(&pcf, PCF8563_CONTROL_STATUS2_WRITE, &tmp);
}

/**
 * @brief Will return the current set alarm as a tm struct.
 * 
 * @return tm 
 */
tm RTC::getRTCAlarm()
{
    tm rtc_alarm = {};

    pcf8563_ioctl(&pcf, PCF8563_ALARM_READ, &rtc_alarm);
    return rtc_alarm;
}

/**
 * @brief Sets a timer of n seconds
 * 
 * @param seconds 
 */
void RTC::setTimer(uint8_t seconds){
    uint8_t control;

    control = PCF8563_TIMER_ENABLE | PCF8563_TIMER_1HZ;

    pcf8563_ioctl(&pcf, PCF8563_TIMER_WRITE, &seconds);
    pcf8563_ioctl(&pcf, PCF8563_TIMER_CONTROL_WRITE, &control);

    /* Prints "Timer!" every 10 seconds. */
    // while (1) {
    //     pcf8563_ioctl(&pcf, PCF8563_CONTROL_STATUS2_READ, &control);
    //     /* Check for timer flag. */
    //     if (control & PCF8563_TF) {
    //         printf("Timer!\n");

    //         /* Clear timer flag. */
    //         tmp &= ~PCF8563_TF;
    //         pcf8563_ioctl(&pcf, PCF8563_CONTROL_STATUS2_WRITE, &tmp);
    //     }
    // }
}
