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
| TFT | ST7789V | all | LovyanGFX | working[^1] |
| Touchscreen | FT6336 | all | LovyanGFX | working[^2] |
| PMU | AXP202 | all | axp202 component | working[^3] |
| RTC | PCF5863 | all | pcf5863 component | working[^3] |
| PSRAM | 4MB/8MB[^3] | all | esp-idf menuconfig | partially working[^4] |
| Acceleration | BMA423 | all | - | to be implemented[^5] |
| Infrared | - | all | - | to be implemented[^5] |
| PCM amplifier | MAX98357A | v1 & v3 | - | to be implemented[^5] |
| GPS (v2) |Quectel L76K | v2 | - | to be implemented[^5] |
| Vibration | IO | v1 & v3 | - | to be implemented[^5] |
| Vibration | DRV2605 | v2 | - | to be implemented[^5] |
| Button | AXP202 | all | - | working[^3] |

[^1]: Feature complete (no known issues)
[^2]: Touch seems slightly off on screen borders.
[^3]: Should work but needs more testing and propper functions for better usage.
[^4]: Watch v1 and v3 have 8MB, v2 has 4MB. During initialization v1 will only detect 4MB of its 8MB.
[^5]: No efforts of implementation until now. Could be in early stage or no stage at all.
