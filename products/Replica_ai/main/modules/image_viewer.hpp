/*
 * SPDX-FileCopyrightText: 2025 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: CC0-1.0
 */
#pragma once

#include <stdint.h>
#include <stdbool.h>
#include "lvgl.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct image_viewer *image_viewer_handle_t;

/**
 * @brief 初始化图片查看器
 *
 * @return image_viewer_handle_t 图片查看器句柄，失败返回NULL
 */
image_viewer_handle_t image_viewer_init(void);

/**
 * @brief 去初始化图片查看器
 *
 * @param viewer 图片查看器句柄
 */
void image_viewer_deinit(image_viewer_handle_t viewer);

/**
 * @brief 启动图片轮询显示任务
 *
 * @param viewer 图片查看器句柄
 * @return esp_err_t
 */
esp_err_t image_viewer_start(image_viewer_handle_t viewer);

/**
 * @brief 停止图片轮询显示
 *
 * @param viewer 图片查看器句柄
 * @return esp_err_t
 */
esp_err_t image_viewer_stop(image_viewer_handle_t viewer);

#ifdef __cplusplus
}
#endif
