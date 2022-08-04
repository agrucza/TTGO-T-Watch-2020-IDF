/**
 * @file main.cpp
 * @author agrucza (a.grucza@gmx.net)
 * @brief PlatformIO ESP-IDF project for LilyGo TTGO-T-Watch-2020
 * @version 0.1
 * @date 2022-07-18
 * 
 * @copyright Copyright (c) 2022
 * 
 */

#include <stdio.h>

#include "TTGO_TWATCH_2020.hpp"

TTGO_TWATCH_2020 ttgo;

static const char *TAG = "main";

extern "C"
{
    /**
     * @brief main entry point and startup funtion
     * 
     */
    void app_main()
    {
        ttgo.init();
        
        static int32_t x,y;
        
        while(1)
        {
            if (ttgo.getLcd().getTouch(&x, &y))
            {
                ttgo.getLcd().clearDisplay(TFT_BLACK);
                ttgo.getLcd().fillRect(x-2, y-2, 5, 5, TFT_RED);
                ttgo.getLcd().setCursor(0,0);
                ttgo.getLcd().printf("Touch:(%03d,%03d)", x, y);
            }
            
            vTaskDelay(50 / portTICK_PERIOD_MS);
        }
    }
}
