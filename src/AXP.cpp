/**
 * @file AXP.cpp
 * @author agrucza (a.grucza@gmx.net)
 * @brief This class will provide access to the AXP202 chip
 * @version 0.1
 * @date 2022-07-18
 * 
 * @copyright Copyright (c) 2022
 * 
 */

#include <math.h>
#include <esp_sleep.h>
#include <esp_log.h>
#include <i2c_helper.h>

#include "AXP.hpp"
#include "Event.hpp"

#include "device_config.hpp"

axp202_t            AXP::axp;
bool                AXP::sleep      = false;
std::array<std::array<void(*)(), 8>, AXP202_IRQ_REG_NUM> AXP::irqFunctions;

/**
 * @brief initialization of APX202 I2C connection and init chip
 * 
 */
void AXP::init()
{
    ESP_LOGI(TAG, "Initializing AXP202");

    axp.read = &i2c_read;
    axp.write = &i2c_write;
    axp.handle = &i2c_port;

    if(axp202_init(&AXP::axp) == AXP202_OK)
    {
        ESP_LOGD(TAG, "AXP init successful");
    } else {
        ESP_LOGD(TAG, "AXP init failed");
    }

    // set all needed IRQs on AXP202
    // i guess we wont need all of them so this will be adjusted later
    uint8_t irqVal = 0xFF;
    axp.write(axp.handle, AXP202_ADDRESS, AXP202_ENABLE_CONTROL_1, &irqVal, 1);
    axp.write(axp.handle, AXP202_ADDRESS, AXP202_ENABLE_CONTROL_2, &irqVal, 1);
    axp.write(axp.handle, AXP202_ADDRESS, AXP202_ENABLE_CONTROL_3, &irqVal, 1);
    axp.write(axp.handle, AXP202_ADDRESS, AXP202_ENABLE_CONTROL_4, &irqVal, 1);
    axp.write(axp.handle, AXP202_ADDRESS, AXP202_ENABLE_CONTROL_5, &irqVal, 1);

    /*
     * IRQ setup
     */
    gpio_config_t io_conf;
    // enable gpio interrupt for falling edge
    io_conf.intr_type = GPIO_INTR_NEGEDGE;
    // set gpio to input
    io_conf.mode = GPIO_MODE_INPUT;
    // bit mask of the gpio to use
    io_conf.pin_bit_mask = _BV(TTGO_AXP202_INT);
    // disable pull-down mode
    io_conf.pull_down_en = GPIO_PULLDOWN_DISABLE;
    // disable pull-up mode
    io_conf.pull_up_en = GPIO_PULLUP_DISABLE;
    gpio_config(&io_conf);

    // reset all current IRQ flags to reset IRQ gpio to be in normat state
    clearIRQ();
}

void AXP::irqTask(void* arg)
{

    const char  *TAG        = "AXP IRQ TASK";
    uint32_t    io_num;

    for(;;) {
        if(xQueueReceive(Event::axpEventQueue, &io_num, 0)) {
            // It seems we need to add a small delay here as otherwise some register values will get lost.
            //
            // I checked with 0 & 100ms delay but i could only get the "PEK falling edge" state
            // but no "PEK short press" state resulting in a constant negative GPIO as i did not touch
            // the missing register value.
            //
            // Probably the loop over the registers is inefficient but with a 150ms delay it seems to
            // work fine so far.
            vTaskDelay(150 / portTICK_PERIOD_MS);

            uint8_t reg;
            
            // read all IRQ status registers
            for (uint8_t irqStatusIndex = 0; irqStatusIndex < AXP202_IRQ_REG_NUM; irqStatusIndex++) {
                bool statusFlagExecuteFunction[8] = {false, false, false, false, false, false, false, false};
                uint8_t regReset = 0x00;
                axp.read(axp.handle, AXP202_ADDRESS, AXP202_IRQ_STATUS_1 + irqStatusIndex, &reg, 1);

                // do nothing if register has no bit set (no IRQ)
                if(reg != 0x00)
                {
                    for(int irqStatusFlag = 0; irqStatusFlag<8; irqStatusFlag++)
                    {
                        // check if irq is not set, if so go to next flag
                        if(!(reg & (irqStatusFlag>0?(0x01 << irqStatusFlag):0x01)))
                        {
                            continue;
                        }
                        
                        ESP_LOGD(
                            TAG,
                            "[reg:0x%x][regValue:0x%x][bit:%i] Got IRQ",
                            AXP202_IRQ_STATUS_1 + irqStatusIndex,
                            reg,
                            irqStatusFlag
                        );

                        if(irqFunctions[irqStatusIndex][irqStatusFlag])
                        {
                            // Callback function should be set so execute the function.
                            statusFlagExecuteFunction[irqStatusFlag] = true;
                        }

                        // Collect the status registers value for the found IRQ flag.
                        regReset |= (0x01 << irqStatusFlag);
                    }
                    // Write
                    axp.write(axp.handle, AXP202_ADDRESS, AXP202_IRQ_STATUS_1 + irqStatusIndex, &regReset, 1);
                    for(int irqStatusFlag = 0; irqStatusFlag<8; irqStatusFlag++)
                    {
                        if(statusFlagExecuteFunction[irqStatusFlag])
                        {
                            irqFunctions[irqStatusIndex][irqStatusFlag]();
                        }
                    }
                }
            }
        }
        vTaskDelay(100 / portTICK_PERIOD_MS);
    }
}

bool AXP::setIrqFunction(uint8_t irqReg, uint8_t irqFlag, void(*newIrqFunction)())
{
    const char *TAG = "AXP setIrqFunction";
    
    // transform irqReg and irqFlag to the index values of the array
    // e.g.: AXP202_IRQ_STATUS_1 (0x48) - AXP202_IRQ_STATUS_1 (0x48)
    // will result in 0 (first index)
    irqReg = irqReg - AXP202_IRQ_STATUS_1;
    irqFlag = log2(irqFlag);
    ESP_LOGD(TAG, "Setting IRQ Function to [irqReg:0x%x][irqFlag:%u]", AXP202_IRQ_STATUS_1 + irqReg, irqFlag);
    if(irqReg < 0 && irqReg > AXP202_IRQ_REG_NUM)
    {
        ESP_LOGD(TAG, "Error while setting IRQ function: IRQ Register out of bounce (0x%x)", AXP202_IRQ_STATUS_1 + irqReg);
        return false;
    }
    AXP::irqFunctions[irqReg][irqFlag] = newIrqFunction;
    return true;
}


/**
 * @brief Load all register values to its access variables
 * 
 */
void AXP::getRegisterValues()
{
    axp202_read(&axp, AXP202_ACIN_VOLTAGE, &vacin);
    axp202_read(&axp, AXP202_ACIN_CURRENT, &iacin);
    axp202_read(&axp, AXP202_VBUS_VOLTAGE, &vvbus);
    axp202_read(&axp, AXP202_VBUS_CURRENT, &ivbus);
    axp202_read(&axp, AXP202_TEMP, &temp);
    axp202_read(&axp, AXP202_TS_INPUT, &vts);
    axp202_read(&axp, AXP202_GPIO0_VOLTAGE, &vgpio0);
    axp202_read(&axp, AXP202_GPIO1_VOLTAGE, &vgpio1);
    axp202_read(&axp, AXP202_BATTERY_POWER, &pbat);
    axp202_read(&axp, AXP202_BATTERY_VOLTAGE, &vbat);
    axp202_read(&axp, AXP202_CHARGE_CURRENT, &icharge);
    axp202_read(&axp, AXP202_DISCHARGE_CURRENT, &idischarge);
    axp202_read(&axp, AXP202_IPSOUT_VOLTAGE, &ipsout);
    axp202_read(&axp, AXP202_COULOMB_COUNTER, &cbat);
    axp202_read(&axp, AXP202_FUEL_GAUGE, &fuel);

    axp202_ioctl(&axp, AXP202_READ_POWER_STATUS, &power);
    axp202_ioctl(&axp, AXP202_READ_CHARGE_STATUS, &charge);

    axp202_ioctl(&axp, AXP202_COULOMB_COUNTER_ENABLE, NULL);
    axp202_read(&axp, AXP202_COULOMB_COUNTER, &cbat);
}

/**
 * @brief log all register variables
 * 
 */
void AXP::logStats()
{
    getRegisterValues();

    ESP_LOGD(
        TAG,
        "vacin: %.2fV iacin: %.2fA vvbus: %.2fV ivbus: %.2fA vts: %.2fV temp: %.0fC "
        "vgpio0: %.2fV vgpio1: %.2fV pbat: %.2fmW vbat: %.2fV icharge: %.2fA "
        "idischarge: %.2fA, ipsout: %.2fV cbat: %.2fmAh fuel: %.0f%%",
        vacin, iacin, vvbus, ivbus, vts, temp, vgpio0, vgpio1, pbat, vbat, icharge,
        idischarge, ipsout, cbat, fuel
    );

    ESP_LOGD(TAG, "power: 0x%02x charge: 0x%02x", power, charge);
    ESP_LOGD(TAG, "cbat: %.2fmAh", cbat);
}

void AXP::clearIRQ(void)
{
    uint8_t val = 0xFF;
    
    for (int i = 0; i < AXP202_IRQ_REG_NUM; i++) {
        axp.write(axp.handle, AXP202_ADDRESS, AXP202_IRQ_STATUS_1 + i, &val, 1);
    }

    //memset(_irq, 0, sizeof(_irq));
}
