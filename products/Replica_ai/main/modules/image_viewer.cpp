/*
 * SPDX-FileCopyrightText: 2025 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: CC0-1.0
 */
#include <stdio.h>
#include <string.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/event_groups.h"
#include "esp_log.h"
#include "image_viewer.hpp"
#include "assets/images/esp_brookesia_speaker_assets.h"
#include "bsp/display.h"

static const char *TAG = "IMAGE_VIEWER";

#define IMAGE_VIEWER_STACK_SIZE  (8192)
#define IMAGE_VIEWER_PRIORITY    (5)
#define IMAGE_VIEWER_DELAY_MS    (2000)  // 每张图片显示2秒
#define IMAGE_VIEWER_EVENT_GROUP (BIT0)

typedef struct image_viewer {
    lv_obj_t *screen;           // LVGL屏幕对象
    lv_obj_t *image_obj;        // LVGL图片对象
    EventGroupHandle_t events;  // 事件组
    bool is_running;            // 运行标志
} image_viewer_t;

// 图片数组（22张）
static const lv_image_dsc_t *g_images[] = {
    &speaker_image_middle_app_launcher_default_112_112,
    &speaker_image_middle_quick_settings_battery_charge_20_20,
    &speaker_image_middle_quick_settings_battery_level1_20_20,
    &speaker_image_middle_quick_settings_battery_level2_20_20,
    &speaker_image_middle_quick_settings_battery_level3_20_20,
    &speaker_image_middle_quick_settings_battery_level4_20_20,
    &speaker_image_middle_quick_settings_bluetooth_48_48,
    &speaker_image_middle_quick_settings_brightness_auto_48_48,
    &speaker_image_middle_quick_settings_brightness_high_48_48,
    &speaker_image_middle_quick_settings_brightness_low_48_48,
    &speaker_image_middle_quick_settings_brightness_medium_48_48,
    &speaker_image_middle_quick_settings_lock_48_48,
    &speaker_image_middle_quick_settings_settings_48_48,
    &speaker_image_middle_quick_settings_volume_high_48_48,
    &speaker_image_middle_quick_settings_volume_low_48_48,
    &speaker_image_middle_quick_settings_volume_medium_48_48,
    &speaker_image_middle_quick_settings_volume_off_48_48,
    &speaker_image_middle_quick_settings_wifi_48_48,
    &speaker_image_middle_quick_settings_wifi_close_20_20,
    &speaker_image_middle_quick_settings_wifi_level1_20_20,
    &speaker_image_middle_quick_settings_wifi_level2_20_20,
    &speaker_image_middle_quick_settings_wifi_level3_20_20,
};

static const int g_image_count = sizeof(g_images) / sizeof(g_images[0]);

// 图片轮询显示任务
static void image_viewer_task(void *param)
{
    image_viewer_handle_t viewer = (image_viewer_handle_t)param;
    int current_index = 0;

    ESP_LOGI(TAG, "Image viewer task started");

    // 创建图片对象（只创建一次）
    viewer->image_obj = lv_image_create(viewer->screen);
    lv_obj_center(viewer->image_obj);

    while (viewer->is_running)
    {
        // 显示当前图片
        const lv_image_dsc_t *img_dsc = g_images[current_index];

        // 获取LVGL互斥锁并更新图片源（避免渲染冲突）
        lv_display_t *disp = lv_obj_get_display(viewer->screen);
        lv_lock();

        lv_image_set_src(viewer->image_obj, img_dsc);

        lv_unlock();

        // 获取图片尺寸并打印日志
        ESP_LOGI(TAG, "Displaying image [%d/%d]: size %dx%d, stride %d",
                 current_index + 1, g_image_count,
                 img_dsc->header.w, img_dsc->header.h, img_dsc->header.stride);

        // 更新到下一张图片
        current_index = (current_index + 1) % g_image_count;

        // 延迟指定时间
        vTaskDelay(pdMS_TO_TICKS(IMAGE_VIEWER_DELAY_MS));
    }

    ESP_LOGI(TAG, "Image viewer task stopped");
    vTaskDelete(NULL);
}

image_viewer_handle_t image_viewer_init(void)
{
    image_viewer_t *viewer = (image_viewer_t *)malloc(sizeof(image_viewer_t));
    if (!viewer)
    {
        ESP_LOGE(TAG, "Failed to allocate memory for image viewer");
        return NULL;
    }

    memset(viewer, 0, sizeof(image_viewer_t));

    // 创建事件组
    viewer->events = xEventGroupCreate();
    if (!viewer->events)
    {
        ESP_LOGE(TAG, "Failed to create event group");
        free(viewer);
        return NULL;
    }

    // 获取当前活动屏幕
    viewer->screen = lv_scr_act();
    viewer->image_obj = NULL;
    viewer->is_running = false;

    // 设置屏幕背景色为黑色（需要加锁避免渲染冲突）
    lv_lock();
    lv_obj_set_style_bg_color(viewer->screen, lv_color_black(), 0);
    lv_obj_set_style_bg_opa(viewer->screen, LV_OPA_COVER, 0);
    lv_unlock();

    ESP_LOGI(TAG, "Image viewer initialized with %d images", g_image_count);

    return viewer;
}

void image_viewer_deinit(image_viewer_handle_t viewer)
{
    if (!viewer)
    {
        return;
    }

    // 停止任务
    image_viewer_stop(viewer);

    // 清理LVGL对象
    if (viewer->image_obj)
    {
        lv_obj_del(viewer->image_obj);
    }

    // 清理事件组
    if (viewer->events)
    {
        vEventGroupDelete(viewer->events);
    }

    free(viewer);
    ESP_LOGI(TAG, "Image viewer deinitialized");
}

esp_err_t image_viewer_start(image_viewer_handle_t viewer)
{
    if (!viewer)
    {
        ESP_LOGE(TAG, "Invalid viewer handle");
        return ESP_ERR_INVALID_ARG;
    }

    if (viewer->is_running)
    {
        ESP_LOGW(TAG, "Image viewer is already running");
        return ESP_OK;
    }

    viewer->is_running = true;

    // 创建图片显示任务
    BaseType_t ret = xTaskCreate(
        image_viewer_task,
        "image_viewer",
        IMAGE_VIEWER_STACK_SIZE,
        viewer,
        IMAGE_VIEWER_PRIORITY,
        NULL);

    if (ret != pdPASS)
    {
        ESP_LOGE(TAG, "Failed to create image viewer task");
        viewer->is_running = false;
        return ESP_FAIL;
    }

    ESP_LOGI(TAG, "Image viewer started");

    return ESP_OK;
}

esp_err_t image_viewer_stop(image_viewer_handle_t viewer)
{
    if (!viewer)
    {
        ESP_LOGE(TAG, "Invalid viewer handle");
        return ESP_ERR_INVALID_ARG;
    }

    if (!viewer->is_running)
    {
        ESP_LOGW(TAG, "Image viewer is not running");
        return ESP_OK;
    }

    viewer->is_running = false;
    ESP_LOGI(TAG, "Image viewer stop requested");

    return ESP_OK;
}
