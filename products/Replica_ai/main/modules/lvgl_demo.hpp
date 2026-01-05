/*
 * SPDX-FileCopyrightText: 2025 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef LVGL_DEMO_HPP
#define LVGL_DEMO_HPP

#ifdef __cplusplus
extern "C" {
#endif

#include <stdbool.h>

/**
 * @brief LVGL 示例类型
 */
typedef enum {
    LVGL_DEMO_WIDGETS,           /* 控件示例（按钮、标签、滑块等）*/
    LVGL_DEMO_BENCHMARK,         /* 性能测试示例 */
    LVGL_DEMO_MUSIC,            /* 音乐播放器示例 */
    LVGL_DEMO_STRESS,           /* 压力测试示例 */
    LVGL_DEMO_FLEX_LAYOUT,      /* Flexbox 布局示例 */
    LVGL_DEMO_TRANSFORM,         /* 变换动画示例 */
    LVGL_DEMO_SCROLL,           /* 滚动效果示例 */
    LVGL_DEMO_MULTILANG,        /* 多语言示例 */
    LVGL_DEMO_VECTOR_GRAPHIC,   /* 矢量图形示例 */
    LVGL_DEMO_RENDER,           /* 渲染效果示例 */
    LVGL_DEMO_KEYPAD_ENCODER,   /* 键盘和编码器示例 */
    LVGL_DEMO_CUSTOM,           /* 自定义示例 */
    LVGL_DEMO_MAX
} lvgl_demo_type_t;

/**
 * @brief 运行 LVGL 示例
 *
 * @param demo_type 示例类型
 * @return true 成功运行
 * @return false 运行失败
 */
bool lvgl_demo_run(lvgl_demo_type_t demo_type);

/**
 * @brief 运行自定义 LVGL 示例
 *
 * @return true 成功运行
 * @return false 运行失败
 */
bool lvgl_demo_run_custom(void);

/**
 * @brief 停止当前运行的 LVGL 示例
 */
void lvgl_demo_stop(void);

#ifdef __cplusplus
}
#endif

#endif /* LVGL_DEMO_HPP */
