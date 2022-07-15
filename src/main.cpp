#include <stdio.h>
#include <driver/rtc_io.h>
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>

#include "LGFX_TWATCH_2020.hpp"

static LGFX_TWATCH_2020 lcd;

extern "C"
{
    void app_main()
    {
        lcd.init();
        lcd.clearDisplay(TFT_RED);
        
        static int32_t x,y;
        
        while(1)
        {
            if (lcd.getTouch(&x, &y))
            {
                lcd.clearDisplay(TFT_BLACK);
                lcd.fillRect(x-2, y-2, 5, 5, TFT_RED);
                lcd.setCursor(0,0);
                lcd.printf("Touch:(%03d,%03d)", x,y);

                
            }
        }
    }
}