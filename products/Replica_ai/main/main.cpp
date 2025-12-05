/*
 * SPDX-FileCopyrightText: 2010-2022 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: CC0-1.0
 */

#include <stdio.h>

#ifdef ESP_UTILS_LOG_TAG
#undef ESP_UTILS_LOG_TAG
#endif
#define ESP_UTILS_LOG_TAG "Main"

constexpr bool EXAMPLE_SHOW_MEM_INFO = false;

extern "C" void app_main()
{
    printf("Hello world!\n");
    printf("Hello world 2 !\n");
    printf("Hello world 3 !\n");
}
