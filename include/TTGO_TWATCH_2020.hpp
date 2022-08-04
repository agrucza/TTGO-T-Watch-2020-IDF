#pragma once

/**
 * @file TTGO_TWATCH_2020.hpp
 * @author agrucza (a.grucza@gmx.net)
 * @brief main class for watch
 * @version 0.1
 * @date 2022-07-18
 * 
 * @copyright Copyright (c) 2022
 * 
 */

#include <i2c_helper.h>
#include "LGFX_TWATCH_2020.hpp"

#include "AXP.hpp"
#include "RTC.hpp"
#include "Event.hpp"

/**
 * @brief Basic init class for TTGO-T-Watch-2020
 * 
 */
class TTGO_TWATCH_2020
{
    const char          *TAG        = "TTGO_TWATCH_2020";

    LGFX_TWATCH_2020    lcd;
    i2c_port_t          i2c_port    = I2C_NUM_0;
    AXP                 axp;
    RTC                 rtc;
    Event               event;
public:
    TTGO_TWATCH_2020(void){}
    void init();
    LGFX_TWATCH_2020 getLcd(){ return lcd; }
    esp_err_t nvs_init();
};
