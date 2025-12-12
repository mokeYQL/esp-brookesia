/*
 * SPDX-FileCopyrightText: 2025 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: CC0-1.0
 */
#pragma once

#include "esp_codec_dev.h"

// 音频初始化
bool audio_init();

// 获取音频设备句柄，供system.cpp使用
esp_codec_dev_handle_t get_play_device();
esp_codec_dev_handle_t get_rec_device();
