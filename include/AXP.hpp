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

#include <array>
#include <axp202.h>
#include <i2c_helper.h>

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
    static xQueueHandle     eventQueue;
    static bool             sleep;
    /*
     * i know this is quite ugly to produce a multi dim array like this
     * it would be better to introduce an new multi dim array template in the future
     */
    static std::array<std::array<void(*)(), 8>, AXP202_IRQ_REG_NUM> irqFunctions;

    AXP(void){}
    void                    init();
    static axp202_t         getAXP(){ return axp; }
    static void             irqTask(void* arg);
    static void             clearIRQ(void);
    static bool             setIrqFunction(uint8_t irqReg, uint8_t irqFlag, void(*newIrqFunction)());
    void                    getRegisterValues();
    void                    logStats();
};
