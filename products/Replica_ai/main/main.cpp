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
#include "modules/display.hpp"
#include "modules/lvgl_demo.hpp"
#include "modules/animation_manager.hpp"

// Include generated asset headers
#include "assets/animations/mmap_generate_anim_boot.h"
#include "assets/animations/mmap_generate_anim_emotion.h"
#include "assets/animations/mmap_generate_anim_icon.h"

constexpr bool EXAMPLE_SHOW_MEM_INFO = true;
constexpr bool default_dummy_draw = false;

extern "C" void app_main()
{

    assert(display_init(default_dummy_draw) && "Initialize display failed");

    /* 初始化动画管理器 */
    animation_manager_handle_t anim_mgr = animation_manager_init();
    if (!anim_mgr)
    {
        ESP_LOGE("Main", "Failed to initialize animation manager");
        return;
    }

    ESP_LOGI("Main", "Animation manager initialized successfully");

    /* 创建动画演示任务 */
    BaseType_t ret = xTaskCreate(
        animation_demo_task,
        "anim_demo",
        8192,
        anim_mgr,
        5,
        NULL);

    if (ret != pdPASS)
    {
        ESP_LOGE("Main", "Failed to create animation demo task");
        animation_manager_deinit(anim_mgr);
    }

    /* 运行 LVGL 示例 */
    // lvgl_demo_run(LVGL_DEMO_WIDGETS);
    // lvgl_demo_run(LVGL_DEMO_BENCHMARK);
    // lvgl_demo_run(LVGL_DEMO_KEYPAD_ENCODER);
    // lvgl_demo_run(LVGL_DEMO_CUSTOM);
    // lvgl_demo_run(LVGL_DEMO_STRESS);
    // lvgl_demo_run(LVGL_DEMO_RENDER);

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
}
