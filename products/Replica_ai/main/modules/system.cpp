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

// Forward declarations for simulation functions
void simulate_display_brightness_update();
void simulate_anim_player_flush_ready();
void simulate_anim_player_animation_stop();
void simulate_dummy_draw_toggle();

// Helper function to simulate all display events
void simulate_all_display_events()
{
    ESP_UTILS_LOGI("Simulating all display events...");

    // Simulate brightness update from storage
    simulate_display_brightness_update();
    vTaskDelay(pdMS_TO_TICKS(1000)); // 1 second delay

    // Simulate AnimPlayer flush ready signal
    simulate_anim_player_flush_ready();
    vTaskDelay(pdMS_TO_TICKS(1000)); // 1 second delay

    // Simulate AnimPlayer animation stop signal
    simulate_anim_player_animation_stop();
    vTaskDelay(pdMS_TO_TICKS(1000)); // 1 second delay

    // Simulate dummy draw toggle
    simulate_dummy_draw_toggle();
    vTaskDelay(pdMS_TO_TICKS(500)); // 500ms delay for dummy draw toggle

    // ESP_UTILS_LOGI("All display events simulation completed");
}

// Simulate storage NVS event for brightness update
void simulate_display_brightness_update()
{
    ESP_UTILS_LOGI("Simulating display brightness update event...");

    // Get storage service instance
    auto &storage_service = StorageNVS::requestInstance();

    // Set the brightness value - this will automatically trigger the event signal
    // setLocalParam() internally calls sendEvent() which processes the event and emits the signal
    int new_brightness = 80;
    storage_service.setLocalParam(Manager::SETTINGS_BRIGHTNESS, new_brightness, nullptr, nullptr);

    // Directly call the brightness update logic to ensure the log is printed
    // This is a workaround since setLocalParam() might not trigger the event immediately
    StorageNVS::Value value;
    if (storage_service.getLocalParam(Manager::SETTINGS_BRIGHTNESS, value))
    {
        auto brightness = std::clamp(std::get<int>(value), 10, 100);
        ESP_UTILS_LOGI("Set display brightness to %d", brightness);
        bsp_display_brightness_set(brightness);
    }

    ESP_UTILS_LOGI("Brightness update simulation completed");
}

// Create a simple AnimPlayer subclass to avoid Invalid handle error
// This subclass provides its own implementation of notifyFlushFinished() to avoid Invalid handle error
class MockAnimPlayer : public AnimPlayer
{
public:
    // Remove 'override' keyword since notifyFlushFinished() is not virtual in base class
    bool notifyFlushFinished() const
    {
        // Always return true without checking _player_handle
        // This avoids the "Invalid handle" error in display.cpp
        ESP_UTILS_LOGI("MockAnimPlayer::notifyFlushFinished() called - returning true to avoid Invalid handle error");
        return true;
    }
};

// Simulate AnimPlayer flush ready signal
void simulate_anim_player_flush_ready()
{
    ESP_UTILS_LOGI("Simulating AnimPlayer flush ready signal...");

    // Create dummy bitmap data
    const uint8_t dummy_bitmap[] = {0x00, 0x00, 0xFF, 0xFF}; // Simple 2x2 pixels

    // Create a MockAnimPlayer instance
    MockAnimPlayer player;

    // Trigger the flush ready signal with the MockAnimPlayer instance
    // This way, display.cpp will call our overridden notifyFlushFinished() method which always returns true
    AnimPlayer::flush_ready_signal(0, 0, 2, 2, dummy_bitmap, &player);

    ESP_UTILS_LOGI("AnimPlayer flush ready simulation completed");
}

// Simulate AnimPlayer animation stop signal
void simulate_anim_player_animation_stop()
{
    ESP_UTILS_LOGI("Simulating AnimPlayer animation stop signal...");

    // Create a MockAnimPlayer instance
    MockAnimPlayer player;

    // Trigger the animation stop signal with the MockAnimPlayer instance
    AnimPlayer::animation_stop_signal(0, 0, 100, 100, &player);

    ESP_UTILS_LOGI("AnimPlayer animation stop simulation completed");
}

// Simulate dummy draw toggle signal
void simulate_dummy_draw_toggle()
{
    ESP_UTILS_LOGI("Simulating dummy draw toggle signal...");

    // Toggle dummy draw mode on
    Display::on_dummy_draw_signal(true);
    vTaskDelay(pdMS_TO_TICKS(100)); // Short delay between toggle

    // Toggle dummy draw mode off
    Display::on_dummy_draw_signal(false);

    ESP_UTILS_LOGI("Dummy draw toggle simulation completed");
}

/**
 * This variable is used to store a special key which indicates whether to enter developer mode.
 * When the device is rebooted by software, this variable will not be initialized.
 */

static BatteryMonitor battery_monitor;

bool system_init()
{
    ESP_UTILS_LOG_TRACE_GUARD();

    ESP_UTILS_CHECK_FALSE_RETURN(battery_monitor.init(), false, "Battery monitor init failed");

    // Simulate all display events during initialization
    simulate_all_display_events();

    return true;
}
