/**
 * @file Buzzer.cpp
 * @author a.grucza (a.grucza@gmx.net)
 * @brief 
 * @version 0.1
 * @date 2022-08-14
 * 
 * @copyright Copyright (c) 2022
 * 
 */

#include "Buzzer.hpp"
#include "Event.hpp"

void Buzzer::init()
{
    // setup buzzer
    gpio_set_direction(TTGO_MOTOR, GPIO_MODE_OUTPUT);
}

void Buzzer::task(void* arg)
{

    const char      *TAG        = "EVENT BUZZER TASK";
    buzzer_t  buzzerType;

    for(;;) {
        if(xQueueReceive(Event::buzzerEventQueue, &buzzerType, 0)) {
            gpio_set_level(TTGO_MOTOR, 1);
            
            switch(buzzerType.type)
            {
                case buzzer_type::BUZZER_SHORT:
                    vTaskDelay(100 / portTICK_PERIOD_MS);
                break;
                case buzzer_type::BUZZER_SHORT_CONTINUOUS:
                break;
                case buzzer_type::BUZZER_LONG:
                    vTaskDelay(1000 / portTICK_PERIOD_MS);
                break;
                case buzzer_type::BUZZER_LONG_CONTINUOUS:
                break;
            }

            gpio_set_level(TTGO_MOTOR, 0);
        }
        vTaskDelay(10 / portTICK_PERIOD_MS);
    }
}

void Buzzer::buzz(buzzer_type type, uint8_t duration)
{
    buzzer_t queue_type = {
        .type = type,
        .duration = duration
    };
    xQueueSendFromISR(Event::buzzerEventQueue, &queue_type, NULL);
}

