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

#include <esp_log.h>
#include <i2c_helper.h>

#include "AXP.hpp"

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

    axp202_init(&axp);
    // AXP202_ioctl(&axp, AXP202_COULOMB_COUNTER_ENABLE, NULL);
    // AXP202_ioctl(&axp, AXP202_COULOMB_COUNTER_CLEAR, NULL);
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
