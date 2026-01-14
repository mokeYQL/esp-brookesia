/*
 * SPDX-FileCopyrightText: 2025 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: CC0-1.0
 */

#ifndef _WIFI_CONNECT_H_
#define _WIFI_CONNECT_H_

#include "esp_err.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief 初始化 WiFi 连接
 * 
 * 该函数将：
 * 1. 初始化 TCP/IP 网络接口
 * 2. 创建默认事件循环
 * 3. 初始化 WiFi
 * 4. 连接到配置的 WiFi 网络（SSID: yql, Password: 12345678）
 * 5. 等待连接完成
 * 
 * @note 在调用任何网络相关功能（如 Coze 对话）之前必须先调用此函数
 * 
 * @return
 *       - ESP_OK   成功
 *       - 其他      ESP 错误码
 */
esp_err_t wifi_connect_init(void);

/**
 * @brief 等待 WiFi 连接完成
 *
 * @param timeout_ms 超时时间（毫秒）
 *
 * @return
 *       - ESP_OK   连接成功
 *       - ESP_ERR_TIMEOUT  超时
 *       - 其他      ESP 错误码
 */
esp_err_t wifi_connect_wait(uint32_t timeout_ms);

/**
 * @brief 初始化 SNTP 时间同步系统
 *
 * 该函数初始化 SNTP 服务，但不启动。需要在 WiFi 连接前调用。
 *
 * @return
 *       - ESP_OK   初始化成功
 *       - 其他      ESP 错误码
 */
esp_err_t wifi_sync_time_init(void);

/**
 * @brief 启动 SNTP 时间同步
 *
 * 该函数启动 SNTP 服务并等待时间同步完成。
 * 必须在 WiFi 连接成功后调用。
 *
 * @param timeout_ms 超时时间（毫秒）
 *
 * @return
 *       - ESP_OK   时间同步成功
 *       - ESP_ERR_TIMEOUT  超时
 *       - 其他      ESP 错误码
 */
esp_err_t wifi_sync_time_start(uint32_t timeout_ms);

#ifdef __cplusplus
}
#endif

#endif /* _WIFI_CONNECT_H_ */
