#pragma once

#define LGFX_USE_V1

#include <LovyanGFX.hpp>

class LGFX_TWATCH_2020 : public lgfx::LGFX_Device
{
    lgfx::Panel_ST7789  _panel_instance;
    lgfx::Bus_SPI       _bus_instance;
    lgfx::Light_PWM     _light_instance;
    lgfx::Touch_FT5x06  _touch_instance; // FT5206, FT5306, FT5406, FT6206, FT6236, FT6336, FT6436

public:
    LGFX_TWATCH_2020(void)
    {
        {
            // SPI bus control
            auto cfg            = _bus_instance.config();
            cfg.spi_host        = HSPI_HOST;
            cfg.spi_mode        = 0;
            cfg.freq_write      = 80000000;
            cfg.freq_read       = 20000000;
            cfg.spi_3wire       = true;
            cfg.dma_channel     = SPI_DMA_CH_AUTO;
            cfg.pin_sclk        = 18;
            cfg.pin_mosi        = 19;
            cfg.pin_miso        = -1;
            cfg.pin_dc          = 27;

            _bus_instance.config(cfg);
            _panel_instance.setBus(&_bus_instance);
        }
        {
            // Display panel control
            auto cfg            = _panel_instance.config();

            cfg.pin_cs          =     5;
            cfg.pin_rst         =    -1;
            cfg.pin_busy        =    -1;
            // General init values, if unknown comment out or try
            cfg.panel_width     =   240;
            cfg.panel_height    =   240;
            cfg.offset_x        =     0;
            cfg.offset_y        =     0;
            cfg.offset_rotation =     2;
            //cfg.dummy_read_pixel =     8;
            //cfg.dummy_read_bits  =     1;
            cfg.readable        =  true;
            cfg.invert          =  true;
            cfg.rgb_order       = false;
            cfg.dlen_16bit      = false;
            cfg.bus_shared      =  true;

            _panel_instance.config(cfg);
        }
        {
            // Backlight control
            auto cfg            = _light_instance.config();
            cfg.pin_bl          =    12;
            cfg.invert          = false;
            cfg.freq            =  1200;
            cfg.pwm_channel     =     7;

            _light_instance.config(cfg);
            _panel_instance.setLight(&_light_instance);
        }
        {
            // Touch control
            auto cfg            = _touch_instance.config();
            cfg.x_min           =    0;
            cfg.x_max           =  239;
            cfg.y_min           =    0;
            cfg.y_max           =  239;
            cfg.pin_int         =   38;
            cfg.bus_shared      = true;
            cfg.offset_rotation =    2;

            // I2C config
            cfg.i2c_port        =      1;
            cfg.i2c_addr        =   0x38;
            cfg.pin_sda         =     23;
            cfg.pin_scl         =     32;
            cfg.freq            = 400000;

            _touch_instance.config(cfg);
            _panel_instance.setTouch(&_touch_instance);
        }

        setPanel(&_panel_instance);
    }
};
