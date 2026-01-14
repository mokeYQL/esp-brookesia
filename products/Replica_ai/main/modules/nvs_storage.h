/*
 * SPDX-FileCopyrightText: 2025 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: CC0-1.0
 */

#ifndef _NVS_STORAGE_H_
#define _NVS_STORAGE_H_

#include "esp_err.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief 初始化 NVS 存储系统
 * 
 * NVS（Non-Volatile Storage）是 ESP32 的非易失性存储库，
 * 用于保存配置数据。在初始化 WiFi 等网络功能之前必须先初始化 NVS。
 * 
 * 该函数将：
 * 1. 初始化 NVS Flash 存储
 * 2. 如果检测到 NVS 分区损坏或版本不匹配，自动擦除并重新初始化
 * 3. 创建默认命名空间（可选）
 * 
 * @note 必须在 WiFi 初始化之前调用此函数
 * 
 * @return
 *       - ESP_OK   成功
 *       - 其他      ESP 错误码
 */
esp_err_t nvs_storage_init(void);

/**
 * @brief 反初始化 NVS 存储系统
 * 
 * @return
 *       - ESP_OK   成功
 *       - 其他      ESP 错误码
 */
esp_err_t nvs_storage_deinit(void);

#ifdef __cplusplus
}
#endif

#endif /* _NVS_STORAGE_H_ */
