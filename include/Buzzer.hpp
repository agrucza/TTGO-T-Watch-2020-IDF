/**
 * @file Buzzer.hpp
 * @author a.grucza (a.grucza@gmx.net)
 * @brief Buzzer init and functions
 * @version 0.1
 * @date 2022-08-14
 * 
 * @copyright Copyright (c) 2022
 * 
 */

#pragma once

#include <freertos/FreeRTOS.h>
#include <freertos/queue.h>
#include <esp_log.h>

class Buzzer
{
public:
    enum buzzer_type{
        BUZZER_CUSTOM,
        BUZZER_CUSTOM_CONTINUOUS,
        BUZZER_SHORT,
        BUZZER_SHORT_CONTINUOUS,
        BUZZER_LONG,
        BUZZER_LONG_CONTINUOUS
    };

    struct buzzer_t {
        uint8_t type;
        uint8_t duration;
    };

    Buzzer(void){}
    static void init();
    static void task(void* arg);
    static void buzz(buzzer_type type = buzzer_type::BUZZER_SHORT, uint8_t duration = 100);
};
