/**
 * @file Event.cpp
 * @author agrucza (a.grucza@gmx.net)
 * @brief Event implementation
 * @version 0.1
 * @date 2022-07-27
 * 
 * @copyright Copyright (c) 2022
 * 
 */

#include <driver/gpio.h>

#include "Event.hpp"
#include "AXP.hpp"
#include "Buzzer.hpp"
#include "device_config.hpp"

TTGO_TWATCH_2020    *Event::ttgo            = nullptr;
bool                Event::isrInstalled     = false;
xQueueHandle        Event::axpEventQueue    = nullptr;
xQueueHandle        Event::rtcEventQueue    = nullptr;
xQueueHandle        Event::buzzerEventQueue = nullptr;
const uint64_t      Event::sleepTimer       = 2000000;

void Event::init(TTGO_TWATCH_2020* _ttgo)
{
    ttgo = _ttgo;

    if(gpio_install_isr_service(0) == ESP_OK)
    {
        Event::isrInstalled = true;
    }
    
    // setting up AXP queue and ISR handler
    Event::axpEventQueue = xQueueCreate(10, sizeof(uint32_t));
    xTaskCreatePinnedToCore(AXP::irqTask, "axpIrqTask", 4096, NULL, tskIDLE_PRIORITY, NULL, 1);
    gpio_isr_handler_add(TTGO_AXP202_INT, Event::axpISRHandler, (void *) TTGO_AXP202_INT);

    // setup AXP interrupt handler
    AXP::setIrqFunction(AXP202_IRQ_STATUS_3, AXP202_IRQ_STATUS_3_PEKSHORT, &Event::standby);

    // setup buzzer
    Event::buzzerEventQueue = xQueueCreate(5, sizeof(Buzzer::buzzer_t));
    xTaskCreatePinnedToCore(Buzzer::task, "buzzerTask", 1024, NULL, tskIDLE_PRIORITY, NULL, 1);
}

void IRAM_ATTR Event::axpISRHandler(void *arg)
{
    uint32_t gpio_num = (uint32_t) arg;
    xQueueSendFromISR(Event::axpEventQueue, &gpio_num, NULL);
}

void Event::wakeupReason()
{
    esp_sleep_wakeup_cause_t wakeupReason = esp_sleep_get_wakeup_cause();
    ESP_LOGI("WAKEUP", "Wakeup reason: %i", wakeupReason);
}

void Event::standby()
{

    const char *TAG = "EVENT STANDBY CALLBACK";

    uint8_t power = 0x0;

    if(!AXP::sleep)
    {
        AXP::sleep = true;
        ESP_LOGI(TAG, "Going to light standby");

        ESP_LOGD(TAG, "Set backlight off");
        // set backlight to off
        //gpio_set_level((gpio_num_t)TTGO_TFT_BL, 0);

        ESP_LOGD(TAG, "Set LCD to sleep mode");
        // set LCD to sleep mode
        //ttgo->getLcd().writeCommand(0x10);
        ttgo->getLcd().sleep();

        ESP_LOGD(TAG, "Set sleep enable ext0 wakeup");
        esp_sleep_enable_ext0_wakeup(TTGO_AXP202_INT, 0);

        ESP_LOGD(TAG, "Sending AXP202 to standby");
        AXP::getAXP().read(AXP::getAXP().handle, AXP202_ADDRESS, AXP202_SHUTDOWN_VOLTAGE, &power, 1);
        power |= (0x01 << 3);
        AXP::getAXP().write(AXP::getAXP().handle, AXP202_ADDRESS, AXP202_SHUTDOWN_VOLTAGE, &power, 1);

        vTaskDelay(5 / portTICK_PERIOD_MS);
        
        ESP_LOGD(TAG, "SLEEP!");
        esp_light_sleep_start();
    }
    else
    {
        AXP::sleep = false;
        ESP_LOGI(TAG, "Wakeup from sleep");
        power |= (0x03);
        //AXP::getAXP().write(AXP::getAXP().handle, AXP202_ADDRESS, AXP202_SHUTDOWN_VOLTAGE, &power, 1);
        // set LCD to wakeup mode
        ttgo->getLcd().wakeup();
        //vTaskDelay(120 / portTICK_PERIOD_MS);
    }
}