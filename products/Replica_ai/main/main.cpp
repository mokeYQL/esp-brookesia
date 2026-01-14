/*
 * SPDX-FileCopyrightText: 2010-2022 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: CC0-1.0
 */

#include <stdio.h>
#include <cassert>
#include "esp_lib_utils.h"
#include "boost/thread.hpp"
#ifdef ESP_UTILS_LOG_TAG
#undef ESP_UTILS_LOG_TAG
#endif
#define ESP_UTILS_LOG_TAG "Main"

#include "modules/audio_sys.h"
#include "modules/ai_chat.hpp"

// 显示相关
// Include generated asset headers
#include "modules/display.hpp"
// #include "assets/animations/mmap_generate_anim_boot.h"
// #include "assets/animations/mmap_generate_anim_emotion.h"
// #include "assets/animations/mmap_generate_anim_icon.h"
// #include "modules/lvgl_demo.hpp"
// #include "modules/animation_manager.hpp"
// #include "modules/image_viewer.hpp"

constexpr bool EXAMPLE_SHOW_MEM_INFO = true;
constexpr bool default_dummy_draw = false;

// ==============================================================================
// AI 对话事件回调
// ==============================================================================

static void ai_chat_event_handler(ai_chat_event_t event, const char *data, void *user_ctx)
{
    switch (event) {
        case AI_CHAT_EVENT_CHAT_STARTED:
            ESP_UTILS_LOGI("🤖 AI 对话已开始");
            break;
            
        case AI_CHAT_EVENT_CHAT_STOPPED:
            ESP_UTILS_LOGI("⏹️ AI 对话已停止");
            break;
            
        case AI_CHAT_EVENT_CHAT_COMPLETED:
            ESP_UTILS_LOGI("✅ AI 对话已完成");
            break;
            
        case AI_CHAT_EVENT_SPEECH_STARTED:
            ESP_UTILS_LOGI("🔊 AI 开始说话");
            break;
            
        case AI_CHAT_EVENT_SPEECH_STOPPED:
            ESP_UTILS_LOGI("🔇 AI 停止说话");
            break;
            
        case AI_CHAT_EVENT_WAKE_UP:
            ESP_UTILS_LOGI("🎤 AI 已唤醒，可以开始对话");
            break;
            
        case AI_CHAT_EVENT_SLEEP:
            ESP_UTILS_LOGI("😴 AI 已进入睡眠模式");
            break;
            
        case AI_CHAT_EVENT_ERROR:
            ESP_UTILS_LOGE("❌ AI 对话错误: %s", data ? data : "unknown");
            break;
            
        case AI_CHAT_EVENT_SUBTITLE:
            ESP_UTILS_LOGI("📝 字幕: %s", data ? data : "");
            break;
            
        default:
            ESP_UTILS_LOGW("⚠️ 未知 AI 事件: %d", event);
            break;
    }
}

extern "C" void app_main()
{
    ESP_UTILS_LOGI("");
    ESP_UTILS_LOGI("╔═══════════════════════════════════════════╗");
    ESP_UTILS_LOGI("║        ESP32-S3 Replica AI - 智能对话系统          ║");
    ESP_UTILS_LOGI("╚═══════════════════════════════════════════╝");
    ESP_UTILS_LOGI("");

    assert(display_init(default_dummy_draw) && "Initialize display failed");

    // 初始化 AI 对话系统
    ESP_UTILS_LOGI("🚀 初始化 AI 对话系统...");
    if (ai_chat_init(ai_chat_event_handler, NULL) != ESP_OK) {
        ESP_UTILS_LOGE("❌ AI 对话系统初始化失败！");
        return;
    }
    ESP_UTILS_LOGI("✅ AI 对话系统初始化成功！");

    // 启动 AI 对话
    ESP_UTILS_LOGI("▶️  启动 AI 对话...");
    if (ai_chat_start() != ESP_OK) {
        ESP_UTILS_LOGE("❌ AI 对话启动失败！");
        ai_chat_deinit();
        return;
    }
    ESP_UTILS_LOGI("✅ AI 对话已启动！");

    // 显示系统信息
    ESP_UTILS_LOGI("");
    ESP_UTILS_LOGI("📝 系统状态：");
    ESP_UTILS_LOGI("   - 目标芯片: ESP32-S3");
    ESP_UTILS_LOGI("   - CPU 频率: 240 MHz");
    ESP_UTILS_LOGI("   - PSRAM: 8MB (Octal SPI RAM)");
    ESP_UTILS_LOGI("   - Flash: 16MB");
    ESP_UTILS_LOGI("");
    ESP_UTILS_LOGI("═══════════════════════════════════════════");
    ESP_UTILS_LOGI("📝 功能说明：");
    ESP_UTILS_LOGI("   - 🤖 AI 对话：基于 Coze 平台的智能对话");
    ESP_UTILS_LOGI("   - 🎤 唤醒模式：通过唤醒词或命令唤醒 AI");
    ESP_UTILS_LOGI("   - 😴 睡眠模式：让 AI 进入低功耗待机");
    ESP_UTILS_LOGI("   - ⏸️  暂停/恢复：控制 AI 对话状态");
    ESP_UTILS_LOGI("   - 🛑 中断对话：打断当前对话");
    ESP_UTILS_LOGI("═══════════════════════════════════════════");
    ESP_UTILS_LOGI("");

    if constexpr (EXAMPLE_SHOW_MEM_INFO)
    {
        esp_utils::thread_config_guard thread_config({
            .name = "mem_info",
            .stack_size = 4096,
        });
        boost::thread([=]()
                      {
            while (1) {
                esp_utils_mem_print_info();

                audio_sys_get_real_time_stats();

                boost::this_thread::sleep_for(boost::chrono::seconds(20));
            } })
            .detach();
    }

    ESP_UTILS_LOGI("");
    ESP_UTILS_LOGI("🎉 系统启动完成！AI 对话已就绪...");
    ESP_UTILS_LOGI("");
}

// /* 初始化动画管理器（暂时禁用，与图片查看器冲突） */
// animation_manager_handle_t anim_mgr = animation_manager_init();
// if (!anim_mgr)
// {
//     ESP_LOGE("Main", "Failed to initialize animation manager");
//     return;
// }
// ESP_LOGI("Main", "Animation manager initialized successfully");
// /* 创建动画演示任务（暂时禁用） */
// BaseType_t ret = xTaskCreate(
//     animation_demo_task,
//     "anim_demo",
//     8192,
//     anim_mgr,
//     5,
//     NULL);

/* 初始化动画管理器（暂时禁用，与图片查看器冲突） */
// animation_manager_handle_t anim_mgr = animation_manager_init();
// if (!anim_mgr)
// {
//     ESP_LOGE("Main", "Failed to initialize animation manager");
//     return;
// }
// ESP_LOGI("Main", "Animation manager initialized successfully");

/* 创建动画演示任务（暂时禁用） */
// BaseType_t ret = xTaskCreate(
//     animation_demo_task,
//     "anim_demo",
//     8192,
//     anim_mgr,
//     5,
//     NULL);

/* 初始化图片查看器 */
// image_viewer_handle_t img_viewer = image_viewer_init();
// if (!img_viewer)
// {
//     ESP_LOGE("Main", "Failed to initialize image viewer");
//     return;
// }
// ESP_LOGI("Main", "Image viewer initialized successfully");
// esp_err_t ret;
// if (ret != pdPASS)
// {
//     ESP_LOGE("Main", "Failed to create animation demo task");
//     animation_manager_deinit(anim_mgr);
//     image_viewer_deinit(img_viewer);
// }

/* 启动图片轮询显示 */
// ret = image_viewer_start(img_viewer);
// if (ret != ESP_OK)
// {
//     ESP_LOGE("Main", "Failed to start image viewer");
//     image_viewer_deinit(img_viewer);
// }

/* 运行 LVGL 示例 */
// lvgl_demo_run(LVGL_DEMO_WIDGETS);
// lvgl_demo_run(LVGL_DEMO_BENCHMARK);
// lvgl_demo_run(LVGL_DEMO_KEYPAD_ENCODER);
// lvgl_demo_run(LVGL_DEMO_CUSTOM);
// lvgl_demo_run(LVGL_DEMO_STRESS);
// lvgl_demo_run(LVGL_DEMO_RENDER);
