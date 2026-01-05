/*
 * SPDX-FileCopyrightText: 2025 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <cstdio>
#include <cstdint>
#include <cstddef>

#include "lvgl_demo.hpp"
#include "display.hpp"
#include "esp_lib_utils.h"
#include "bsp/echoear.h"

#ifdef ESP_UTILS_LOG_TAG
#undef ESP_UTILS_LOG_TAG
#endif
#define ESP_UTILS_LOG_TAG "LVGL_Demo"

extern "C" {
    #include "lvgl.h"
}

extern "C" {
    /* LVGL 官方示例头文件 */
    #if LV_USE_DEMO_WIDGETS
    #include "demos/widgets/lv_demo_widgets.h"
    #endif

    #if LV_USE_DEMO_BENCHMARK
    #include "demos/benchmark/lv_demo_benchmark.h"
    #endif

    #if LV_USE_DEMO_MUSIC
    #include "demos/music/lv_demo_music.h"
    #endif

    #if LV_USE_DEMO_STRESS
    #include "demos/stress/lv_demo_stress.h"
    #endif

    #if LV_USE_DEMO_FLEX_LAYOUT
    #include "demos/flex_layout/lv_demo_flex_layout.h"
    #endif

    #if LV_USE_DEMO_TRANSFORM
    #include "demos/transform/lv_demo_transform.h"
    #endif

    #if LV_USE_DEMO_SCROLL
    #include "demos/scroll/lv_demo_scroll.h"
    #endif

    #if LV_USE_DEMO_MULTILANG
    #include "demos/multilang/lv_demo_multilang.h"
    #endif

    #if LV_USE_DEMO_VECTOR_GRAPHIC
    #include "demos/vector_graphic/lv_demo_vector_graphic.h"
    #endif

    #if LV_USE_DEMO_RENDER
    #include "demos/render/lv_demo_render.h"
    #endif

    #if LV_USE_DEMO_KEYPAD_AND_ENCODER
    #include "demos/keypad_encoder/lv_demo_keypad_encoder.h"
    #endif
}

/**
 * @brief 运行 LVGL 示例
 */
bool lvgl_demo_run(lvgl_demo_type_t demo_type)
{
    /* 先获取 LVGL 锁 */
    if (!bsp_display_lock(0)) {
        ESP_UTILS_LOGE("Failed to lock display");
        return false;
    }

    /* 根据类型运行不同的示例 */
    switch (demo_type) {
        case LVGL_DEMO_WIDGETS:
#if LV_USE_DEMO_WIDGETS
            ESP_UTILS_LOGI("Running LVGL Widgets demo");
            lv_demo_widgets();
#else
            ESP_UTILS_LOGW("LVGL_DEMO_WIDGETS is not enabled");
            bsp_display_unlock();
            return false;
#endif
            break;

        case LVGL_DEMO_BENCHMARK:
#if LV_USE_DEMO_BENCHMARK
            ESP_UTILS_LOGI("Running LVGL Benchmark demo");
            lv_demo_benchmark();
#else
            ESP_UTILS_LOGW("LVGL_DEMO_BENCHMARK is not enabled");
            bsp_display_unlock();
            return false;
#endif
            break;

        case LVGL_DEMO_MUSIC:
#if LV_USE_DEMO_MUSIC
            ESP_UTILS_LOGI("Running LVGL Music demo");
            lv_demo_music();
#else
            ESP_UTILS_LOGW("LVGL_DEMO_MUSIC is not enabled");
            bsp_display_unlock();
            return false;
#endif
            break;

        case LVGL_DEMO_STRESS:
#if LV_USE_DEMO_STRESS
            ESP_UTILS_LOGI("Running LVGL Stress demo");
            lv_demo_stress();
#else
            ESP_UTILS_LOGW("LVGL_DEMO_STRESS is not enabled");
            bsp_display_unlock();
            return false;
#endif
            break;

        case LVGL_DEMO_FLEX_LAYOUT:
#if LV_USE_DEMO_FLEX_LAYOUT
            ESP_UTILS_LOGI("Running LVGL Flex Layout demo");
            lv_demo_flex_layout();
#else
            ESP_UTILS_LOGW("LVGL_DEMO_FLEX_LAYOUT is not enabled");
            bsp_display_unlock();
            return false;
#endif
            break;

        case LVGL_DEMO_TRANSFORM:
#if LV_USE_DEMO_TRANSFORM
            ESP_UTILS_LOGI("Running LVGL Transform demo");
            lv_demo_transform();
#else
            ESP_UTILS_LOGW("LVGL_DEMO_TRANSFORM is not enabled");
            bsp_display_unlock();
            return false;
#endif
            break;

        case LVGL_DEMO_SCROLL:
#if LV_USE_DEMO_SCROLL
            ESP_UTILS_LOGI("Running LVGL Scroll demo");
            lv_demo_scroll();
#else
            ESP_UTILS_LOGW("LVGL_DEMO_SCROLL is not enabled");
            bsp_display_unlock();
            return false;
#endif
            break;

        case LVGL_DEMO_MULTILANG:
#if LV_USE_DEMO_MULTILANG
            ESP_UTILS_LOGI("Running LVGL Multilang demo");
            lv_demo_multilang();
#else
            ESP_UTILS_LOGW("LVGL_DEMO_MULTILANG is not enabled");
            bsp_display_unlock();
            return false;
#endif
            break;

        case LVGL_DEMO_VECTOR_GRAPHIC:
#if LV_USE_DEMO_VECTOR_GRAPHIC
            ESP_UTILS_LOGI("Running LVGL Vector Graphic demo");
            lv_demo_vector_graphic();
#else
            ESP_UTILS_LOGW("LVGL_DEMO_VECTOR_GRAPHIC is not enabled");
            bsp_display_unlock();
            return false;
#endif
            break;

        case LVGL_DEMO_RENDER:
#if LV_USE_DEMO_RENDER
            ESP_UTILS_LOGI("Running LVGL Render demo");
            lv_demo_render();
#else
            ESP_UTILS_LOGW("LVGL_DEMO_RENDER is not enabled");
            bsp_display_unlock();
            return false;
#endif
            break;

        case LVGL_DEMO_KEYPAD_ENCODER:
#if LV_USE_DEMO_KEYPAD_AND_ENCODER
            ESP_UTILS_LOGI("Running LVGL Keypad Encoder demo");
            lv_demo_keypad_encoder();
#else
            ESP_UTILS_LOGW("LVGL_DEMO_KEYPAD_AND_ENCODER is not enabled");
            bsp_display_unlock();
            return false;
#endif
            break;

        case LVGL_DEMO_CUSTOM:
            /* 运行自定义示例 */
            if (!lvgl_demo_run_custom()) {
                ESP_UTILS_LOGE("Failed to run custom demo");
                bsp_display_unlock();
                return false;
            }
            break;

        default:
            ESP_UTILS_LOGE("Unknown demo type: %d", demo_type);
            bsp_display_unlock();
            return false;
    }

    /* 释放 LVGL 锁 */
    bsp_display_unlock();
    ESP_UTILS_LOGI("LVGL demo started successfully");
    return true;
}

/**
 * @brief 运行自定义 LVGL 示例
 *
 * 这个函数展示了如何创建基本的 LVGL 控件
 */
bool lvgl_demo_run_custom(void)
{
    ESP_UTILS_LOGI("Running custom LVGL demo");

    /* 获取当前活动的屏幕 */
    lv_obj_t *screen = lv_screen_active();

    /* 创建标签 */
    lv_obj_t *label = lv_label_create(screen);
    lv_label_set_text(label, "Hello ESP32-S3!");
    lv_obj_set_style_text_font(label, &lv_font_montserrat_24, 0);
    lv_obj_align(label, LV_ALIGN_TOP_MID, 0, 20);

    /* 创建按钮 */
    lv_obj_t *btn = lv_button_create(screen);
    lv_obj_set_size(btn, 120, 50);
    lv_obj_align(btn, LV_ALIGN_CENTER, 0, 0);

    /* 按钮上的标签 */
    lv_obj_t *btn_label = lv_label_create(btn);
    lv_label_set_text(btn_label, "Click Me");
    lv_obj_center(btn_label);

    /* 添加按钮点击事件回调 */
    lv_obj_add_event_cb(btn, [](lv_event_t *e) {
        lv_obj_t *btn = (lv_obj_t *)lv_event_get_target(e);
        lv_obj_t *label = lv_obj_get_child(btn, 0);
        static int click_count = 0;
        click_count++;
        char buf[32];
        snprintf(buf, sizeof(buf), "Clicked %d times", click_count);
        lv_label_set_text(label, buf);
    }, LV_EVENT_CLICKED, nullptr);

    /* 创建滑块 */
    lv_obj_t *slider = lv_slider_create(screen);
    lv_obj_set_width(slider, 200);
    lv_obj_align(slider, LV_ALIGN_CENTER, 0, 60);
    lv_slider_set_value(slider, 50, LV_ANIM_OFF);

    /* 滑块标签（静态变量用于事件回调） */
    static lv_obj_t *slider_label = nullptr;

    slider_label = lv_label_create(screen);
    lv_label_set_text(slider_label, "Slider: 50");
    lv_obj_align(slider_label, LV_ALIGN_CENTER, 0, 100);

    /* 添加滑块值改变事件回调 */
    lv_obj_add_event_cb(slider, [](lv_event_t *e) {
        lv_obj_t *slider = (lv_obj_t *)lv_event_get_target(e);
        int32_t value = lv_slider_get_value(slider);
        char buf[32];
        snprintf(buf, sizeof(buf), "Slider: %d", (int)value);
        if (slider_label != nullptr) {
            lv_label_set_text(slider_label, buf);
        }
    }, LV_EVENT_VALUE_CHANGED, nullptr);

    /* 创建进度条 */
    lv_obj_t *bar = lv_bar_create(screen);
    lv_obj_set_size(bar, 200, 20);
    lv_obj_align(bar, LV_ALIGN_BOTTOM_MID, 0, -40);
    lv_bar_set_value(bar, 70, LV_ANIM_OFF);

    /* 进度条标签 */
    lv_obj_t *bar_label = lv_label_create(screen);
    lv_label_set_text(bar_label, "Progress: 70%");
    lv_obj_align(bar_label, LV_ALIGN_BOTTOM_MID, 0, -20);

    ESP_UTILS_LOGI("Custom LVGL demo created successfully");
    return true;
}

/**
 * @brief 停止当前运行的 LVGL 示例
 */
void lvgl_demo_stop(void)
{
    ESP_UTILS_LOGI("Stopping LVGL demo");

    if (!bsp_display_lock(0)) {
        ESP_UTILS_LOGE("Failed to lock display");
        return;
    }

    /* 获取当前活动的屏幕 */
    lv_obj_t *screen = lv_screen_active();

    /* 删除屏幕上的所有对象 */
    lv_obj_clean(screen);

    /* 释放 LVGL 锁 */
    bsp_display_unlock();

    ESP_UTILS_LOGI("LVGL demo stopped");
}
