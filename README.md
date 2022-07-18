# TTGO-T-Watch-2020-IDF
The project is based on PlatformIO, ESP-IDF and is an attempt to create an easy to extend firmware for the LilyGO TTGO-T-WATCH-2020.

## Libraries used
- LovyanGFX (PlatformIO ESP-IDF library)

## Components used
- axp202
  - fork of https://github.com/tuupola/axp202
- esp_i2c_helpers
  - fork of https://github.com/tuupola/esp_i2c_helper
- pcf8563
  - fork of https://github.com/tuupola/pcf8563
  
## Special thanks to
- tuupola for providing an easy to understand kitchen sink project as a starting point
  - https://github.com/tuupola/esp_twatch2020/

## Project status
| part | details | watch version | library/component | status |
|---|---|---|---|---|
| TFT | ST7789V | all | LovyanGFX | working |
| Touchscreen | FT6336 | all | LovyanGFX | working (touch seems slightly off on screen borders) |
| PMU | AXP202 | all | axp202 component | working (needs more testing) |
| RTC | PCF5863 | all | pcf5863 component | working (needs more testing) |
| PSRAM | 8MB (v2 has 4MB) | all | esp-idf menuconfig | only 4MB available in v1 |
| Acceleration | BMA423 | all | - | - |
| Infrared | - | all | - | - |
| PCM amplifier | MAX98357A | v1 & v3 | - | - |
| GPS (v2) |Quectel L76K | v2 | - | - |
| Vibration | IO | v1 & v3 | - | - |
| Vibration | DRV2605 | v2 | - | - |
| Button | AXP202 | all | - | - |
