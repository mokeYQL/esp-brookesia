/*
 * SPDX-FileCopyrightText: 2025 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: CC0-1.0
 */

#include <algorithm>
#include <string>

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "nvs_flash.h"

#include "esp_check.h"
#include "esp_err.h"
#include "esp_log.h"
#include "esp_heap_caps.h"

#include "lvgl.h"
#include "esp_lvgl_port.h"
#include "bsp/esp-bsp.h"

namespace
{
    constexpr const char *TAG = "DisplayMini";
    constexpr const char *NVS_NAMESPACE = "display";
    constexpr const char *NVS_KEY_BRIGHTNESS = "brightness";

    constexpr int BRIGHTNESS_MIN = 10;
    constexpr int BRIGHTNESS_MAX = 100;
    constexpr int BRIGHTNESS_DEFAULT = 80;

    lv_obj_t *s_value_label = nullptr;

    int load_brightness_from_nvs()
    {
        nvs_handle_t handle;
        int32_t stored = BRIGHTNESS_DEFAULT;

        esp_err_t err = nvs_open(NVS_NAMESPACE, NVS_READONLY, &handle);
        if (err != ESP_OK)
        {
            return BRIGHTNESS_DEFAULT;
        }

        err = nvs_get_i32(handle, NVS_KEY_BRIGHTNESS, &stored);
        nvs_close(handle);
        if (err != ESP_OK)
        {
            return BRIGHTNESS_DEFAULT;
        }

        return std::clamp(static_cast<int>(stored), BRIGHTNESS_MIN, BRIGHTNESS_MAX);
    }

    void save_brightness_to_nvs(int value)
    {
        nvs_handle_t handle;
        ESP_ERROR_CHECK(nvs_open(NVS_NAMESPACE, NVS_READWRITE, &handle));

        value = std::clamp(value, BRIGHTNESS_MIN, BRIGHTNESS_MAX);
        ESP_ERROR_CHECK(nvs_set_i32(handle, NVS_KEY_BRIGHTNESS, value));
        ESP_ERROR_CHECK(nvs_commit(handle));
        nvs_close(handle);
    }

    void update_label_text(int value)
    {
        if (s_value_label == nullptr)
        {
            return;
        }

        lv_label_set_text_fmt(s_value_label, "ligt:%d%%", value);
    }

    void slider_event_cb(lv_event_t *event)
    {
        auto slider = static_cast<lv_obj_t *>(lv_event_get_target(event));
        if (slider == nullptr)
        {
            return;
        }

        int value = lv_slider_get_value(slider);
        value = std::clamp(value, BRIGHTNESS_MIN, BRIGHTNESS_MAX);

        if (bsp_display_brightness_set(value) == ESP_OK)
        {
            save_brightness_to_nvs(value);
            update_label_text(value);
        }
        else
        {
            ESP_LOGW(TAG, "Failed to set brightness, keep previous value");
        }
    }

    void create_ui(int initial_brightness)
    {
        lv_obj_t *screen = lv_screen_active();
        lv_obj_t *title = lv_label_create(screen);
        lv_label_set_text(title, "Speaker Display Mini");
        lv_obj_align(title, LV_ALIGN_TOP_MID, 0, 40);

        s_value_label = lv_label_create(screen);
        update_label_text(initial_brightness);
        lv_obj_align_to(s_value_label, title, LV_ALIGN_OUT_BOTTOM_MID, 0, 20);

        lv_obj_t *slider = lv_slider_create(screen);
        lv_slider_set_range(slider, BRIGHTNESS_MIN, BRIGHTNESS_MAX);
        lv_slider_set_value(slider, initial_brightness, LV_ANIM_OFF);
        lv_obj_set_width(slider, LV_PCT(80));
        lv_obj_align_to(slider, s_value_label, LV_ALIGN_OUT_BOTTOM_MID, 0, 30);
        lv_obj_add_event_cb(slider, slider_event_cb, LV_EVENT_VALUE_CHANGED, nullptr);

        lv_obj_t *helper = lv_label_create(screen);
        lv_label_set_text(helper, "Drag the slider to save NVS");
        lv_label_set_long_mode(helper, LV_LABEL_LONG_WRAP);
        lv_obj_set_width(helper, LV_PCT(90));
        lv_obj_align_to(helper, slider, LV_ALIGN_OUT_BOTTOM_MID, 0, 20);
    }

    void ensure_nvs_ready()
    {
        esp_err_t ret = nvs_flash_init();
        if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND)
        {
            ESP_ERROR_CHECK(nvs_flash_erase());
            ESP_ERROR_CHECK(nvs_flash_init());
        }
        else
        {
            ESP_ERROR_CHECK(ret);
        }
    }
} // namespace

extern "C" void app_main(void)
{
    ESP_LOGI(TAG, "Boot speaker display mini demo");
    ensure_nvs_ready();

    ESP_ERROR_CHECK(bsp_power_init(true));

    bsp_display_cfg_t cfg = {
        .lvgl_port_cfg = {
            .task_priority = 4,
            .task_stack = 10 * 1024,
            .task_affinity = 1,
            .task_max_sleep_ms = 500,
            .task_stack_caps = MALLOC_CAP_INTERNAL | MALLOC_CAP_8BIT,
            .timer_period_ms = 5,
        },
        .buffer_size = BSP_LCD_H_RES * BSP_LCD_V_RES,
        .double_buffer = true,
        .flags = {
            .buff_spiram = false,
            .default_dummy_draw = false,
        },
    };

    lv_disp_t *disp = bsp_display_start_with_config(&cfg);
    ESP_ERROR_CHECK(disp ? ESP_OK : ESP_FAIL);
    ESP_ERROR_CHECK(bsp_display_backlight_on());

    const int brightness = load_brightness_from_nvs();
    ESP_ERROR_CHECK(bsp_display_brightness_set(brightness));

    ESP_ERROR_CHECK(bsp_display_lock(portMAX_DELAY) ? ESP_OK : ESP_FAIL);
    create_ui(brightness);
    bsp_display_unlock();

    while (true)
    {
        vTaskDelay(pdMS_TO_TICKS(1000));
    }
}
