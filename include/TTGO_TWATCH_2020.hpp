#pragma once

#include <i2c_helper.h>
#include "LGFX_TWATCH_2020.hpp"
#include "AXP.hpp"
#include "RTC.hpp"

class TTGO_TWATCH_2020
{
    const char          *TAG        = "TTGO_TWATCH_2020";

    LGFX_TWATCH_2020    lcd;
    i2c_port_t          i2c_port    = I2C_NUM_0;
    AXP                 axp;
    RTC                 rtc;
public:
    TTGO_TWATCH_2020(void){}
    void init();
    LGFX_TWATCH_2020 getLcd(){ return lcd; }
    esp_err_t nvs_init();
};
