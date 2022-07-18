#pragma once

/**
 * @file AXP.hpp
 * @author agrucza (a.grucza@gmx.net)
 * @brief This class will provide access to the AXP202 chip
 * @version 0.1
 * @date 2022-07-18
 * 
 * @copyright Copyright (c) 2022
 * 
 */

#include <axp202.h>

class AXP
{
    const char          *TAG        = "AXP";
    i2c_port_t          i2c_port    = I2C_NUM_0;
    axp202_t            axp;
    // variables for status registers
    float               vacin, iacin, vvbus, ivbus, vts, vgpio0, vgpio1, temp, pbat;
    float               vbat, icharge, idischarge, ipsout, cbat, fuel;
    uint8_t             power, charge;
public:
    AXP(void){}
    void init();
    axp202_t getAXP(){ return axp; }
    void getRegisterValues();
    void logStats();
};
