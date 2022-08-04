#pragma once

/**
 * @file Event.cpp
 * @author agrucza (a.grucza@gmx.net)
 * @brief This class is defining all events
 * @version 0.1
 * @date 2022-07-27
 * 
 * @copyright Copyright (c) 2022
 * 
 */

#include <freertos/FreeRTOS.h>
#include <freertos/queue.h>
#include <esp_log.h>
#include <esp_sleep.h>

#include "axp202.h"

#include "AXP.hpp"
#include "device_config.hpp"

class Event
{
public:
    static bool             isrInstalled;
    static xQueueHandle     axpEventQueue;
    static xQueueHandle     rtcEventQueue;
    static const uint64_t   sleepTimer;

    static void             init();

    static void IRAM_ATTR   axpISRHandler(void *arg)
    {
        uint32_t gpio_num = (uint32_t) arg;
        xQueueSendFromISR(Event::axpEventQueue, &gpio_num, NULL);
    }

    static void standby()
    {

        const char *TAG = "EVENT STANDBZ CALLBACK";
        ESP_LOGD(TAG, "Going to standby (not)");

        uint8_t power;

        if(!AXP::sleep)
        {
            //uint8_t pin_bl = ((lgfx::Light_PWM*) ttgo.getLcd().getPanel()->getLight())->config().pin_bl;
            AXP::sleep = true;

            AXP::getAXP().read(AXP::getAXP().handle, AXP202_ADDRESS, AXP202_SHUTDOWN_VOLTAGE, &power, 1);
            power |= (0x01 << 3);
            AXP::getAXP().write(AXP::getAXP().handle, AXP202_ADDRESS, AXP202_SHUTDOWN_VOLTAGE, &power, 1);

            esp_sleep_enable_ext0_wakeup(TTGO_AXP202_INT, 0);
            // timer wakeup every 2 seconds

            //ttgo.getLcd().getPanel()->setSleep(true);
            esp_sleep_enable_timer_wakeup(Event::sleepTimer);
            
            esp_light_sleep_start();
        }
    }
};
