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

#define _BV(b)                          (1ULL << b)
#define REG_IS_ENABLED(reg, channel)    (bool)(reg & _BV(channel))

#define AXP202_ERROR            (-2)
#define AXP202_ERROR_RW         (-3)
#define AXP202_ERROR_SETTING    (-4)

class AXP
{
    const char              *TAG        = "AXP";
    i2c_port_t              i2c_port    = I2C_NUM_0;
    // variables for status registers
    float                   vacin, iacin, vvbus, ivbus, vts, vgpio0, vgpio1, temp, pbat;
    float                   vbat, icharge, idischarge, ipsout, cbat, fuel;
    uint8_t                 power, charge;
public:
    static axp202_t         axp;
    static xQueueHandle eventQueue;
    AXP(void){}
    void                    init();
    axp202_t                getAXP(){ return axp; }

    static void IRAM_ATTR   isrHandler(void *arg);
    static void             irqTask(void* arg);
    static void             clearIRQ(void);

    void                    getRegisterValues();
    void                    logStats();
};
