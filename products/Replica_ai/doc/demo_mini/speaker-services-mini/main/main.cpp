/*
 * SPDX-FileCopyrightText: 2025 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: CC0-1.0
 */

#include <string>
#include <variant>

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#include "esp_check.h"
#include "esp_log.h"

#include "esp_brookesia.hpp"
#include "esp_lib_utils.h"

using namespace esp_brookesia::services;

namespace {
constexpr const char *TAG = "ServicesMini";

constexpr const char *KEY_BRIGHTNESS = "demo_brightness";
constexpr const char *KEY_PROFILE = "demo_profile";

constexpr int BRIGHTNESS_DEFAULT = 60;
constexpr int BRIGHTNESS_STEP = 10;
constexpr int BRIGHTNESS_MIN = 10;
constexpr int BRIGHTNESS_MAX = 100;

void log_event(const StorageNVS::Event &event)
{
    ESP_LOGI(
        TAG,
        "[Event] operation=%d key=%s",
        static_cast<int>(event.operation),
        event.key.empty() ? "None" : event.key.c_str()
    );
}

void periodic_update_task(void *)
{
    auto &storage = StorageNVS::requestInstance();
    int current = BRIGHTNESS_DEFAULT;
    bool toggle_profile = false;

    while (true) {
        current += BRIGHTNESS_STEP;
        if (current > BRIGHTNESS_MAX) {
            current = BRIGHTNESS_MIN;
        }
        StorageNVS::Value brightness = current;
        ESP_ERROR_CHECK(storage.setLocalParam(KEY_BRIGHTNESS, brightness) ? ESP_OK : ESP_FAIL);

        const char *profile_name = toggle_profile ? "assistant" : "music";
        StorageNVS::Value profile = std::string(profile_name);
        ESP_ERROR_CHECK(storage.setLocalParam(KEY_PROFILE, profile) ? ESP_OK : ESP_FAIL);

        ESP_LOGI(TAG, "Updated settings -> brightness=%d, profile=%s", current, profile_name);
        toggle_profile = !toggle_profile;
        vTaskDelay(pdMS_TO_TICKS(5000));
    }
}

int get_or_set_default_int(StorageNVS &storage, const char *key, int default_value)
{
    StorageNVS::Value value = default_value;
    if (!storage.getLocalParam(key, value)) {
        ESP_LOGW(TAG, "Key %s not found, using default %d", key, default_value);
        ESP_ERROR_CHECK(storage.setLocalParam(key, value) ? ESP_OK : ESP_FAIL);
        return default_value;
    }

    if (auto ptr = std::get_if<int>(&value)) {
        return *ptr;
    }

    ESP_LOGW(TAG, "Key %s has unexpected type, reset to default", key);
    StorageNVS::Value new_value = default_value;
    ESP_ERROR_CHECK(storage.setLocalParam(key, new_value) ? ESP_OK : ESP_FAIL);
    return default_value;
}

std::string get_or_set_default_str(StorageNVS &storage, const char *key, const std::string &default_value)
{
    StorageNVS::Value value = default_value;
    if (!storage.getLocalParam(key, value)) {
        ESP_LOGW(TAG, "Key %s not found, using default %s", key, default_value.c_str());
        ESP_ERROR_CHECK(storage.setLocalParam(key, value) ? ESP_OK : ESP_FAIL);
        return default_value;
    }

    if (auto ptr = std::get_if<std::string>(&value)) {
        return *ptr;
    }

    ESP_LOGW(TAG, "Key %s has unexpected type, reset to default", key);
    StorageNVS::Value new_value = default_value;
    ESP_ERROR_CHECK(storage.setLocalParam(key, new_value) ? ESP_OK : ESP_FAIL);
    return default_value;
}
} // namespace

extern "C" void app_main(void)
{
    ESP_LOGI(TAG, "Boot speaker services mini demo");

    auto &storage = StorageNVS::requestInstance();
    ESP_ERROR_CHECK(storage.begin() ? ESP_OK : ESP_FAIL);
    storage.connectEventSignal(log_event);

    int brightness = get_or_set_default_int(storage, KEY_BRIGHTNESS, BRIGHTNESS_DEFAULT);
    auto profile = get_or_set_default_str(storage, KEY_PROFILE, "assistant");
    ESP_LOGI(TAG, "Initial settings -> brightness=%d, profile=%s", brightness, profile.c_str());

    xTaskCreatePinnedToCore(periodic_update_task, "svc_update", 4096, nullptr, 5, nullptr, tskNO_AFFINITY);

    while (true) {
        vTaskDelay(pdMS_TO_TICKS(10000));
    }
}
