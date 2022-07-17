#include <stdio.h>
#include <esp_log.h>

#include "TTGO_TWATCH_2020.hpp"

TTGO_TWATCH_2020 ttgo;

static const char *TAG = "main";

extern "C"
{
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
                ttgo.getLcd().printf("Touch:(%03d,%03d)", x,y);
            }
        }
    }
}
