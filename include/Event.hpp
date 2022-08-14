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

#include "TTGO_TWATCH_2020.hpp"
#include "AXP.hpp"
#include "device_config.hpp"

class Event
{
public:
    static TTGO_TWATCH_2020     *ttgo;
    static bool                 isrInstalled;
    static xQueueHandle         axpEventQueue;
    static xQueueHandle         rtcEventQueue;
    static const uint64_t       sleepTimer;

    static void                 init(TTGO_TWATCH_2020* _ttgo);
    static void IRAM_ATTR       axpISRHandler(void *arg);
    static void                 wakeupReason();

    static void standby();
};
