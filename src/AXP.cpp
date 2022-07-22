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

#include "device_config.hpp"

axp202_t        AXP::axp;
xQueueHandle    AXP::eventQueue = NULL;

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
    
    //change gpio intrrupt type for one pin
    //gpio_set_intr_type(TTGO_AXP202_INT, GPIO_INTR_ANYEDGE);

    //create a queue to handle gpio event from isr
    AXP::eventQueue = xQueueCreate(10, sizeof(uint32_t));

    xTaskCreate(AXP::irqTask, "axpIrqTask", 2048, NULL, 10, NULL);
    //install gpio isr service
    gpio_install_isr_service(0);
    //hook isr handler for specific gpio pin
    gpio_isr_handler_add(TTGO_AXP202_INT, AXP::isrHandler, (void *) TTGO_AXP202_INT);

    // reset all current IRQ flags to reset IRQ gpio to be in normat state
    clearIRQ();
}

void AXP::irqTask(void* arg)
{
    uint32_t io_num;
    for(;;) {
        if(xQueueReceive(AXP::eventQueue, &io_num, 0)) {
            ESP_LOGD("AXP IRQ Task Handler", "GPIO[%d] intr, val: %d\n", io_num, gpio_get_level((gpio_num_t)io_num));
            AXP::clearIRQ();
        }
        vTaskDelay(100 / portTICK_PERIOD_MS);
    }
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

void IRAM_ATTR AXP::isrHandler(void *arg)
{
    uint32_t gpio_num = (uint32_t) arg;
    xQueueSendFromISR(AXP::eventQueue, &gpio_num, NULL);
}

void AXP::clearIRQ(void)
{
    uint8_t val = 0xFF;
    
    for (int i = 0; i < AXP202_IRQ_REG_NUM; i++) {
        axp.write(axp.handle, AXP202_ADDRESS, AXP202_IRQ_STATUS_1 + i, &val, 1);
    }

    //memset(_irq, 0, sizeof(_irq));
}