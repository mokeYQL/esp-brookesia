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

constexpr bool EXAMPLE_SHOW_MEM_INFO = false;
constexpr bool default_dummy_draw = false;
extern "C" void app_main()
{

    assert(display_init(default_dummy_draw) && "Initialize display failed");

    /* 运行 LVGL 示例 */
    lvgl_demo_run(LVGL_DEMO_WIDGETS);

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
