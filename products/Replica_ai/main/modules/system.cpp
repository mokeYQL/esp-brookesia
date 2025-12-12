/*
 * SPDX-FileCopyrightText: 2025 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: CC0-1.0
 */
#include <string>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "soc/soc_caps.h"
#if SOC_USB_SERIAL_JTAG_SUPPORTED
#include "soc/usb_serial_jtag_reg.h"
#include "hal/usb_serial_jtag_ll.h"
#endif
#include "esp_private/usb_phy.h"
#include "bsp/esp-bsp.h"
#include "esp_brookesia.hpp"
#include "esp_brookesia_app_settings.hpp"
#ifdef ESP_UTILS_LOG_TAG
#undef ESP_UTILS_LOG_TAG
#endif
#define ESP_UTILS_LOG_TAG "System"
#include "esp_lib_utils.h"
// #include "coze_agent_config.h"
// #include "coze_agent_config_default.h"
// #include "usb_msc.h"
#include "system.hpp"

#include "battery_monitor.h"
// #include "imu_gesture.h"
// #include "touch_sensor.h"
// #include "led_indicator.h"

constexpr const char *FUNCTION_OPEN_APP_THREAD_NAME = "open_app";
constexpr int FUNCTION_OPEN_APP_THREAD_STACK_SIZE = 20 * 1024;
constexpr int FUNCTION_OPEN_APP_WAIT_SPEAKING_PRE_MS = 2000;
constexpr int FUNCTION_OPEN_APP_WAIT_SPEAKING_INTERVAL_MS = 10;
constexpr int FUNCTION_OPEN_APP_WAIT_SPEAKING_MAX_MS = 2000;
constexpr bool FUNCTION_OPEN_APP_THREAD_STACK_CAPS_EXT = true;

constexpr const char *FUNCTION_VOLUME_CHANGE_THREAD_NAME = "volume_change";
constexpr size_t FUNCTION_VOLUME_CHANGE_THREAD_STACK_SIZE = 6 * 1024;
constexpr bool FUNCTION_VOLUME_CHANGE_THREAD_STACK_CAPS_EXT = true;
constexpr int FUNCTION_VOLUME_CHANGE_STEP = 20;

constexpr const char *FUNCTION_BRIGHTNESS_CHANGE_THREAD_NAME = "brightness_change";
constexpr size_t FUNCTION_BRIGHTNESS_CHANGE_THREAD_STACK_SIZE = 6 * 1024;
constexpr bool FUNCTION_BRIGHTNESS_CHANGE_THREAD_STACK_CAPS_EXT = true;
constexpr int FUNCTION_BRIGHTNESS_CHANGE_STEP = 30;

constexpr int DEVELOPER_MODE_KEY = 0x655;

using namespace esp_brookesia;
using namespace esp_brookesia::systems::speaker;
using namespace esp_brookesia::gui;
using namespace esp_brookesia::apps;
using namespace esp_brookesia::services;
using namespace esp_brookesia::ai_framework;

/**
 * This variable is used to store a special key which indicates whether to enter developer mode.
 * When the device is rebooted by software, this variable will not be initialized.
 */

static BatteryMonitor battery_monitor;

bool system_init()
{
    ESP_UTILS_LOG_TRACE_GUARD();

    ESP_UTILS_CHECK_FALSE_RETURN(battery_monitor.init(), false, "Battery monitor init failed");

    return true;
}
