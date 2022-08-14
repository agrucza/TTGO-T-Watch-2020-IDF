#pragma once

#define TTGO_TWATCH_2020_V1
//#define TTGO_TWATCH_2020_V2
//#define TTGO_TWATCH_2020_V3

#define TTGO_I2C_SDA                21
#define TTGO_I2C_SCL                22
#define TTGO_I2C_PORT_NUM            1
#define TTGO_I2C_FREQUENCY      100000

#define TTGO_TFT_RST                -1
#define TTGO_TFT_MISO               -1
#define TTGO_TFT_MOSI               19
#define TTGO_TFT_SCLK               18
#define TTGO_TFT_DC                 27
#define TTGO_TFT_CS                  5
#define TTGO_TFT_RST                -1

#define TTGO_FT6336_SDA             23
#define TTGO_FT6336 SCL             32
#define TTGO_FT6336_INT             38

#define TTGO_BMA423_INT             39
#define TTGO_PCF8563_INT            37
#define TTGO_AXP202_INT (gpio_num_t)35

#if defined(TTGO_TWATCH_2020_V1)
#define TTGO_TFT_BL                 12
#endif

#if defined(TTGO_TWATCH_2020_V2)
#define TTGO_TFT_BL                 25
#define TTGO_FT6336_RESET           TTGO_AXP202_EXTEN
#define TTGO_GPS_1PPS               34
#define TTGO_GPS_TX                 26
#define TTGO_GPS_RX                 36
#define TTGO_GPS_WAKEUP             33
#define TTGO_IR_SEND                 2
#endif

#if defined(TTGO_TWATCH_2020_V3)
#define TTGO_TFT_BL                 15
#define TTGO_FT6336_RST             14
#define TTGO_PDM_DATA                2
#define TTGO_PDM_CLK                 0
#endif


#if defined(TTGO_TWATCH_2020_V1) || defined(TTGO_TWATCH_2020_V3)
#define TTGO_MOTOR       (gpio_num_t)4
#define TTGO_I2S_BCK                26
#define TTGO_I2S_WS                 25
#define TTGO_I2S_DOUT               33
#define TTGO_IR_SEND                13
#endif
