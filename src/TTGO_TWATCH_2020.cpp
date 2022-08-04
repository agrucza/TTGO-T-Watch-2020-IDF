/**
 * @file TTGO_TWATCH_2020.cpp
 * @author agrucza (a.grucza@gmx.net)
 * @brief main class for watch
 * @version 0.1
 * @date 2022-07-18
 * 
 * @copyright Copyright (c) 2022
 * 
 */

#include "TTGO_TWATCH_2020.hpp"

#include <time.h>
#include <esp_log.h>
#include <nvs_flash.h>

#include <i2c_helper.h>
#include <axp202.h>

/**
 * @brief Watch init method. Will init all additional hardware
 * 
 */
void TTGO_TWATCH_2020::init()
{

    ESP_LOGI(TAG, "SDK version: %s", esp_get_idf_version());
    ESP_LOGI(TAG, "Heap when starting: %d", esp_get_free_heap_size());

    ESP_LOGI(TAG, "Initializing I2C");
    i2c_init(i2c_port);
    
    // init AXP202
    axp.init();

    // init RTC
    rtc.init();

    // init Events
    event.init();

    // init LovyanGFX
    lcd.init();
    lcd.clearDisplay(TFT_RED);
    
    ESP_LOGI(TAG, "Initializing non volatile storage");
    nvs_init();

    //ESP_LOGI(TAG, "Initializing wifi");
    //wifi_init();

    ESP_LOGI(TAG, "Start SNTP sync");
    /* Set your POSIX timezone here. */
    // setenv("TZ", "EET-3", 1);
    // tzset();

    // sntp_setoperatingmode(SNTP_OPMODE_POLL);
    // sntp_setservername(0, "pool.ntp.org");
    // sntp_set_sync_mode(SNTP_SYNC_MODE_IMMED);
    // sntp_set_time_sync_notification_cb(sntp_set_rtc);
    // sntp_init();

    ESP_LOGI(TAG, "Heap after init: %d", esp_get_free_heap_size());

    // xTaskCreatePinnedToCore(rtc_task, "RTC", 2048, NULL, 1, NULL, 1);
    // xTaskCreatePinnedToCore(log_task, "Log", 4096, NULL, 2, NULL, 1);
    // xTaskCreatePinnedToCore(backbuffer_task, "Backbuffer", 8192, NULL, 1, NULL, 0);
}

/**
 * @brief Will init the flash.
 * 
 * @return esp_err_t 
 */
esp_err_t TTGO_TWATCH_2020::nvs_init()
{
    esp_err_t status = nvs_flash_init();
    if (status == ESP_ERR_NVS_NO_FREE_PAGES || status == ESP_ERR_NVS_NEW_VERSION_FOUND) {
        ESP_ERROR_CHECK(nvs_flash_erase());
        status = nvs_flash_init();
    }
    ESP_ERROR_CHECK_WITHOUT_ABORT(status);
    return status;
}
