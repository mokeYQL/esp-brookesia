/*
 * SPDX-FileCopyrightText: 2025 Espressif Systems (Shanghai) CO., LTD
 *
 * SPDX-License-Identifier: CC0-1.0
 */

#ifndef _AI_CHAT_H_
#define _AI_CHAT_H_

#include "esp_err.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief  AI 对话事件类型
 */
typedef enum {
    AI_CHAT_EVENT_CHAT_STARTED = 0,      /*!< 对话开始 */
    AI_CHAT_EVENT_CHAT_STOPPED,       /*!< 对话停止 */
    AI_CHAT_EVENT_CHAT_COMPLETED,     /*!< 对话完成 */
    AI_CHAT_EVENT_SPEECH_STARTED,     /*!< AI 开始说话 */
    AI_CHAT_EVENT_SPEECH_STOPPED,     /*!< AI 停止说话 */
    AI_CHAT_EVENT_WAKE_UP,             /*!< 唤醒状态 */
    AI_CHAT_EVENT_SLEEP,              /*!< 睡眠状态 */
    AI_CHAT_EVENT_ERROR,               /*!< 错误事件 */
    AI_CHAT_EVENT_SUBTITLE,            /*!< 字幕事件 */
} ai_chat_event_t;

/**
 * @brief  AI 对话事件回调函数类型
 *
 * @param[in]  event  事件类型
 * @param[in]  data   事件数据（可选）
 * @param[in]  ctx    用户上下文
 */
typedef void (*ai_chat_event_callback_t)(ai_chat_event_t event, const char *data, void *ctx);

/**
 * @brief  初始化 AI 对话系统
 *
 * @param[in]  event_cb  事件回调函数
 * @param[in]  ctx       用户上下文
 *
 * @return
 *       - ESP_OK   成功
 *       - 其他      ESP 错误码
 */
esp_err_t ai_chat_init(ai_chat_event_callback_t event_cb, void *ctx);

/**
 * @brief  反初始化 AI 对话系统
 *
 * @return
 *       - ESP_OK   成功
 *       - 其他      ESP 错误码
 */
esp_err_t ai_chat_deinit(void);

/**
 * @brief  启动 AI 对话
 *
 * @return
 *       - ESP_OK   成功
 *       - 其他      ESP 错误码
 */
esp_err_t ai_chat_start(void);

/**
 * @brief  停止 AI 对话
 *
 * @return
 *       - ESP_OK   成功
 *       - 其他      ESP 错误码
 */
esp_err_t ai_chat_stop(void);

/**
 * @brief  暂停 AI 对话
 *
 * @return
 *       - ESP_OK   成功
 *       - 其他      ESP 错误码
 */
esp_err_t ai_chat_pause(void);

/**
 * @brief  恢复 AI 对话
 *
 * @return
 *       - ESP_OK   成功
 *       - 其他      ESP 错误码
 */
esp_err_t ai_chat_resume(void);

/**
 * @brief  唤醒 AI（进入对话模式）
 *
 * @return
 *       - ESP_OK   成功
 *       - 其他      ESP 错误码
 */
esp_err_t ai_chat_wakeup(void);

/**
 * @brief  让 AI 进入睡眠模式
 *
 * @return
 *       - ESP_OK   成功
 *       - 其他      ESP 错误码
 */
esp_err_t ai_chat_sleep(void);

/**
 * @brief  中断当前对话
 *
 * @return
 *       - ESP_OK   成功
 *       - 其他      ESP 错误码
 */
esp_err_t ai_chat_interrupt(void);

#ifdef __cplusplus
}
#endif

#endif /* _AI_CHAT_H_ */
