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
#include "device_config.hpp"

bool            Event::isrInstalled     = false;

xQueueHandle    Event::axpEventQueue    = nullptr;
xQueueHandle    Event::rtcEventQueue    = nullptr;
const uint64_t  Event::sleepTimer       = 2000000;

void Event::init()
{
    if(gpio_install_isr_service(0) == ESP_OK)
    {
        Event::isrInstalled = true;
    }
    
    // setting up AXP queue and ISR handler
    Event::axpEventQueue = xQueueCreate(10, sizeof(uint32_t));
    xTaskCreatePinnedToCore(AXP::irqTask, "axpIrqTask", 2048, NULL, tskIDLE_PRIORITY, NULL, 1);
    gpio_isr_handler_add(TTGO_AXP202_INT, Event::axpISRHandler, (void *) TTGO_AXP202_INT);

    // setup AXP interrupt handler
    AXP::setIrqFunction(AXP202_IRQ_STATUS_3, AXP202_IRQ_STATUS_3_PEKSHORT, &Event::standby);
}
