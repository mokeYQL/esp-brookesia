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

#include "bsp/esp-bsp.h"
#include "i2c_bus.h"
#include "bq27220.h"
#include "esp_brookesia.hpp"

using namespace esp_brookesia::services;

namespace
{

    constexpr const char *TAG = "PowerMini";
    constexpr int BATTERY_MONITOR_INTERVAL_MS = 1000; // 电池监控间隔
    constexpr int LOW_BATTERY_THRESHOLD = 10;         // 低电量阈值（百分比）
    constexpr int CRITICAL_BATTERY_THRESHOLD = 5;     // 临界电量阈值（百分比）

    bq27220_handle_t s_battery_handle = nullptr;
    i2c_bus_handle_t s_i2c_bus = nullptr;

    // 使用与speaker项目相同的CEDV配置
    static const ParamCEDV g_cedv = {
        .cedv_conf = {
            .gauge_conf = {
                .CCT = 1,
                .CSYNC = 0,
                .EDV_CMP = 0,
                .SC = 1,
                .FIXED_EDV0 = 0,
                .FCC_LIM = 1,
                .FC_FOR_VDQ = 1,
                .IGNORE_SD = 1,
                .SME0 = 0,
            },
        },
        .full_charge_cap = 650,
        .design_cap = 650,
        .reserve_cap = 0,
        .near_full = 200,
        .self_discharge_rate = 20,
        .EDV0 = 3490,
        .EDV1 = 3511,
        .EDV2 = 3535,
        .EMF = 3670,
        .C0 = 115,
        .R0 = 968,
        .T0 = 4547,
        .R1 = 4764,
        .TC = 11,
        .C1 = 0,
        .DOD0 = 4147,
        .DOD10 = 4002,
        .DOD20 = 3969,
        .DOD30 = 3938,
        .DOD40 = 3880,
        .DOD50 = 3824,
        .DOD60 = 3794,
        .DOD70 = 3753,
        .DOD80 = 3677,
        .DOD90 = 3574,
        .DOD100 = 3490,
    };

    // 电池状态结构体
    typedef struct
    {
        uint16_t voltage;       // 电压（mV）
        int16_t current;        // 电流（mA，充电为正，放电为负）
        uint16_t soc;           // 电量百分比（0-100%）
        uint16_t capacity;      // 剩余容量（mAh）
        uint16_t full_capacity; // 满容量（mAh）
        bool is_charging;       // 是否正在充电
        bool is_full;           // 是否充满
        bool is_low;            // 是否低电量
        bool is_critical;       // 是否临界电量
    } battery_info_t;

    // 初始化I2C总线
    bool init_i2c_bus()
    {
        ESP_LOGI(TAG, "Initializing I2C bus");

        // 使用与speaker项目相同的I2C配置
        const i2c_config_t i2c_bus_conf = {
            .mode = I2C_MODE_MASTER,
            .sda_io_num = BSP_I2C_SDA,
            .scl_io_num = BSP_I2C_SCL,
            .sda_pullup_en = GPIO_PULLUP_ENABLE,
            .scl_pullup_en = GPIO_PULLUP_ENABLE,
            .master = {.clk_speed = 400000}, // 400kHz
            .clk_flags = 0,
        };

        s_i2c_bus = i2c_bus_create(I2C_NUM_0, &i2c_bus_conf);
        if (!s_i2c_bus)
        {
            ESP_LOGE(TAG, "Failed to create I2C bus");
            return false;
        }

        ESP_LOGI(TAG, "I2C bus initialized successfully");
        return true;
    }

    // 初始化电池监控芯片
    bool init_battery_monitor()
    {
        ESP_LOGI(TAG, "Initializing battery monitor (bq27220)");

        // 配置bq27220，使用正确的CEDV参数
        bq27220_config_t bq27220_cfg = {
            .i2c_bus = s_i2c_bus,
            .cedv = (ParamCEDV *)(&g_cedv), // 使用与speaker项目相同的CEDV配置
        };

        // 初始化bq27220
        s_battery_handle = bq27220_init(&bq27220_cfg);
        if (!s_battery_handle)
        {
            ESP_LOGE(TAG, "Failed to initialize bq27220");
            return false;
        }

        ESP_LOGI(TAG, "Battery monitor initialized successfully");
        return true;
    }

    // 读取电池状态
    bool read_battery_status(battery_info_t *info)
    {
        if (s_battery_handle == nullptr || info == nullptr)
        {
            return false;
        }

        // 读取电池状态
        BatteryStatus battery_status;
        if (bq27220_get_battery_status(s_battery_handle, &battery_status) != BQ27220_SUCCESS)
        {
            ESP_LOGE(TAG, "Failed to get battery status");
            return false;
        }

        // 读取电压
        info->voltage = bq27220_get_voltage(s_battery_handle);
        if (info->voltage == BQ27220_ERROR)
        {
            ESP_LOGE(TAG, "Failed to get battery voltage");
            return false;
        }

        // 读取电流
        info->current = bq27220_get_current(s_battery_handle);
        if (info->current == BQ27220_ERROR)
        {
            ESP_LOGE(TAG, "Failed to get battery current");
            return false;
        }

        // 读取电量百分比
        info->soc = bq27220_get_state_of_charge(s_battery_handle);
        if (info->soc == BQ27220_ERROR)
        {
            ESP_LOGE(TAG, "Failed to get battery SOC");
            return false;
        }

        // 读取剩余容量
        info->capacity = bq27220_get_remaining_capacity(s_battery_handle);
        if (info->capacity == BQ27220_ERROR)
        {
            ESP_LOGE(TAG, "Failed to get battery remaining capacity");
            return false;
        }

        // 读取满容量
        info->full_capacity = bq27220_get_full_charge_capacity(s_battery_handle);
        if (info->full_capacity == BQ27220_ERROR)
        {
            ESP_LOGE(TAG, "Failed to get battery full capacity");
            return false;
        }

        // 判断充电状态
        info->is_charging = (info->current > 0);
        info->is_full = battery_status.FC;
        info->is_low = (info->soc <= LOW_BATTERY_THRESHOLD);
        info->is_critical = (info->soc <= CRITICAL_BATTERY_THRESHOLD);

        return true;
    }

    // 打印电池状态
    void print_battery_status(const battery_info_t *info)
    {
        if (info == nullptr)
        {
            return;
        }

        ESP_LOGI(TAG, "Battery Status:");
        ESP_LOGI(TAG, "  Voltage: %d mV", info->voltage);
        ESP_LOGI(TAG, "  Current: %d mA", info->current);
        ESP_LOGI(TAG, "  SOC: %d%%", info->soc);
        ESP_LOGI(TAG, "  Capacity: %d mAh / %d mAh", info->capacity, info->full_capacity);
        ESP_LOGI(TAG, "  Status: %s, %s, %s, %s",
                 info->is_charging ? "Charging" : "Discharging",
                 info->is_full ? "Full" : "Not Full",
                 info->is_low ? "Low Battery" : "Normal",
                 info->is_critical ? "Critical" : "OK");
    }

    // 低电量处理
    void handle_low_battery(const battery_info_t *info)
    {
        if (info == nullptr)
        {
            return;
        }

        if (info->is_critical)
        {
            ESP_LOGW(TAG, "Critical battery level! Shutting down...");
            // 这里可以添加关机逻辑
            // esp_restart(); // 暂时注释，避免实际关机
        }
        else if (info->is_low)
        {
            ESP_LOGW(TAG, "Low battery level! Please charge soon.");
            // 这里可以添加低电量告警逻辑
        }
    }

    // 电池监控任务
    void battery_monitor_task(void *arg)
    {
        ESP_LOGI(TAG, "Battery monitor task started");

        battery_info_t battery_info;

        while (true)
        {
            // 读取电池状态
            if (read_battery_status(&battery_info))
            {
                // 打印电池状态
                print_battery_status(&battery_info);

                // 处理低电量
                handle_low_battery(&battery_info);
            }

            // 等待下一次监控
            vTaskDelay(pdMS_TO_TICKS(BATTERY_MONITOR_INTERVAL_MS));
        }
    }

    // 初始化电源管理系统
    bool init_power_management()
    {
        ESP_LOGI(TAG, "Initializing power management system");

        // 初始化I2C总线
        if (!init_i2c_bus())
        {
            return false;
        }

        // 初始化电池监控
        if (!init_battery_monitor())
        {
            return false;
        }

        // 创建电池监控任务
        xTaskCreatePinnedToCore(battery_monitor_task, "battery_monitor", 4096, nullptr, 5, nullptr, tskNO_AFFINITY);

        ESP_LOGI(TAG, "Power management system initialized successfully");
        return true;
    }

} // namespace

extern "C" void app_main(void)
{
    ESP_LOGI(TAG, "Boot speaker power mini demo");

    /* Initialize power for peripherals */
    ESP_ERROR_CHECK(bsp_power_init(true));

    /* Initialize StorageNVS service */
    auto &storage_service = StorageNVS::requestInstance();
    ESP_ERROR_CHECK(storage_service.begin() ? ESP_OK : ESP_FAIL);

    /* Initialize power management system */
    ESP_ERROR_CHECK(init_power_management() ? ESP_OK : ESP_FAIL);

    /* Main loop */
    while (true)
    {
        vTaskDelay(pdMS_TO_TICKS(1000));
    }
}
