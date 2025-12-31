/*
 * SPDX-FileCopyrightText: 2025 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: CC0-1.0
 */

#include <algorithm>
#include <string>

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#include "esp_check.h"
#include "esp_log.h"
#include "esp_lcd_touch.h"

#include "bsp/esp-bsp.h"
#include "bsp/touch.h"
#include "esp_brookesia.hpp"

using namespace esp_brookesia::services;

namespace
{

    constexpr const char *TAG = "SensorMini";
    esp_lcd_touch_handle_t s_touch_handle = nullptr;

    // 触摸传感器轮询任务
    void touch_polling_task(void *arg)
    {
        ESP_LOGI(TAG, "Touch polling task started");

        while (true)
        {
            // 读取触摸数据
            esp_err_t ret = esp_lcd_touch_read_data(s_touch_handle);
            if (ret != ESP_OK)
            {
                ESP_LOGE(TAG, "Failed to read touch data: %s", esp_err_to_name(ret));
                vTaskDelay(pdMS_TO_TICKS(100));
                continue;
            }

            // 获取触摸坐标
            uint16_t x = 0, y = 0;
            uint16_t strength = 0;
            uint8_t point_num = 0;
            bool touched = esp_lcd_touch_get_coordinates(s_touch_handle, &x, &y, &strength, &point_num, 1);

            if (touched && point_num > 0)
            {
                ESP_LOGI(TAG, "Touch detected at x=%d, y=%d, strength=%d, points=%d", x, y, strength, point_num);
            }

            vTaskDelay(pdMS_TO_TICKS(100));
        }
    }

    // 初始化触摸传感器
    bool init_touch_sensor()
    {
        ESP_LOGI(TAG, "Initializing touch sensor");

        // 初始化触摸传感器
        bsp_touch_config_t touch_config = {
            .dummy = nullptr, // 目前配置结构只有一个dummy字段
        };

        esp_err_t ret = bsp_touch_new(&touch_config, &s_touch_handle);
        if (ret != ESP_OK)
        {
            ESP_LOGE(TAG, "Failed to initialize touch sensor: %s", esp_err_to_name(ret));
            return false;
        }

        // 创建触摸轮询任务
        xTaskCreatePinnedToCore(touch_polling_task, "touch_poll", 4096, nullptr, 5, nullptr, tskNO_AFFINITY);

        ESP_LOGI(TAG, "Touch sensor initialized successfully");
        return true;
    }

    // 初始化IMU传感器
    bool init_imu_sensor()
    {
        ESP_LOGI(TAG, "Initializing IMU sensor");

        // 初始化I2C总线
        esp_err_t ret = bsp_i2c_init();
        if (ret != ESP_OK)
        {
            ESP_LOGE(TAG, "Failed to initialize I2C: %s", esp_err_to_name(ret));
            return false;
        }

        // 这里可以添加IMU传感器的初始化代码
        // 例如BMI270等IMU传感器的初始化

        ESP_LOGI(TAG, "IMU sensor initialized successfully");
        return true;
    }

    // 传感器演示任务
    void sensor_demo_task(void *)
    {
        // 等待系统初始化完成
        vTaskDelay(pdMS_TO_TICKS(1000));

        ESP_LOGI(TAG, "Starting sensor demo...");

        // 初始化触摸传感器
        if (!init_touch_sensor())
        {
            ESP_LOGE(TAG, "Failed to initialize touch sensor");
        }

        // 初始化IMU传感器
        if (!init_imu_sensor())
        {
            ESP_LOGE(TAG, "Failed to initialize IMU sensor");
        }

        ESP_LOGI(TAG, "Sensor demo initialized! Try touching the sensor or moving the device.");

        // 持续运行，等待传感器事件
        while (true)
        {
            vTaskDelay(pdMS_TO_TICKS(1000));
        }
    }

} // namespace

extern "C" void app_main(void)
{
    ESP_LOGI(TAG, "Boot speaker sensor mini demo");

    /* Initialize power for peripherals */
    ESP_ERROR_CHECK(bsp_power_init(true));

    /* Initialize StorageNVS service */
    auto &storage_service = StorageNVS::requestInstance();
    ESP_ERROR_CHECK(storage_service.begin() ? ESP_OK : ESP_FAIL);

    /* Create sensor demo task */
    xTaskCreatePinnedToCore(sensor_demo_task, "sensor_demo", 4096, nullptr, 4, nullptr, tskNO_AFFINITY);

    /* Main loop */
    while (true)
    {
        vTaskDelay(pdMS_TO_TICKS(1000));
    }
}
