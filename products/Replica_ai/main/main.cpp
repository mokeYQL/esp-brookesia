/*
 * SPDX-FileCopyrightText: 2010-2022 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: CC0-1.0
 */

#include <stdio.h>
#include "esp_lib_utils.h"

#include "boost/thread.hpp"

#ifdef ESP_UTILS_LOG_TAG
#undef ESP_UTILS_LOG_TAG
#endif
#define ESP_UTILS_LOG_TAG "Main"

#include "modules/audio_sys.h"
#include "modules/display.hpp"
#include "modules/services.hpp"
#include "modules/led_indicator.h"
#include "modules/file_system.hpp"
#include "modules/system.hpp"

constexpr bool EXAMPLE_SHOW_MEM_INFO = true;
extern "C" void app_main()
{
    auto default_dummy_draw = true;
    ESP_UTILS_LOG_TRACE_GUARD();
    printf("Project version: %s\n", CONFIG_APP_PROJECT_VER);
    assert(services_init() && "Initialize services failed");
    assert(display_init(default_dummy_draw) && "Initialize display failed");
    assert(led_indicator_init() && "Initialize led indicator failed");
    if (!file_system_init())
    {
        ESP_UTILS_LOGE("Initialize file system failed, related features will be disabled");
    }
    // assert(system_init() && "Initialize system failed");
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

                boost::this_thread::sleep_for(boost::chrono::seconds(5));
            } })
            .detach();
    }
}
