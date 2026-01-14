/*
 * SPDX-FileCopyrightText: 2025 Espressif Systems (Shanghai) CO., LTD
 *
 * SPDX-License-Identifier: CC0-1.0
 */

#include <string.h>
#include <time.h>
#include "ai_chat.hpp"
#include "wifi_connect.h"
#include "esp_log.h"
#include "esp_err.h"
#include "esp_coze_chat.h"
#include "esp_coze_utils.h"
#include "http_client_request.h"
#include "esp_random.h"
#include "esp_system.h"
#include "cJSON.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

// 使用 sdkconfig 中的配置

#define COZE_AGENT_ENABLE_DEFAULT_CONFIG CONFIG_EXAMPLE_COZE_AGENT_ENABLE_DEFAULT_CONFIG

#ifdef CONFIG_EXAMPLE_COZE_AGENT_PUBLIC_KEY
#define COZE_PUBLIC_KEY CONFIG_EXAMPLE_COZE_AGENT_PUBLIC_KEY
#else
#define COZE_PUBLIC_KEY ""
#endif

#ifdef CONFIG_EXAMPLE_COZE_AGENT_PRIVATE_KEY_FILE_PATH
#define COZE_PRIVATE_KEY_FILE CONFIG_EXAMPLE_COZE_AGENT_PRIVATE_KEY_FILE_PATH
#else
#define COZE_PRIVATE_KEY_FILE ""
#endif

#ifdef CONFIG_EXAMPLE_COZE_AGENT_APP_ID
#define COZE_APP_ID CONFIG_EXAMPLE_COZE_AGENT_APP_ID
#else
#define COZE_APP_ID ""
#endif

#ifdef CONFIG_EXAMPLE_COZE_AGENT_BOT1_ID
#define COZE_BOT_ID CONFIG_EXAMPLE_COZE_AGENT_BOT1_ID
#else
#define COZE_BOT_ID ""
#endif

#ifdef CONFIG_EXAMPLE_COZE_AGENT_BOT1_NAME
#define COZE_BOT_NAME CONFIG_EXAMPLE_COZE_AGENT_BOT1_NAME
#else
#define COZE_BOT_NAME ""
#endif

#ifdef CONFIG_EXAMPLE_COZE_AGENT_BOT1_VOICE_ID
#define COZE_VOICE_ID CONFIG_EXAMPLE_COZE_AGENT_BOT1_VOICE_ID
#else
#define COZE_VOICE_ID ""
#endif

#if COZE_AGENT_ENABLE_DEFAULT_CONFIG
extern const char private_key_pem_start[] asm("_binary_private_key_pem_start");
extern const char private_key_pem_end[] asm("_binary_private_key_pem_end");
#endif

static const char *TAG = "AI_CHAT";

// AI 对话系统状态
struct ai_chat_state_t
{
    bool initialized;
    bool started;
    bool paused;
    bool sleeping;
    bool wakeup;
    bool speaking;
    esp_coze_chat_handle_t chat_handle;
    TaskHandle_t audio_read_task;
    ai_chat_event_callback_t event_cb;
    void *user_ctx;
};

static struct ai_chat_state_t g_ai_chat = {
    .initialized = false,
    .started = false,
    .paused = false,
    .sleeping = false,
    .wakeup = false,
    .speaking = false,
    .chat_handle = NULL,
    .audio_read_task = NULL,
    .event_cb = NULL,
    .user_ctx = NULL,
};

// 配置信息
static char g_access_token[512] = {0};
static char g_device_id[64] = {0};

// WiFi 连接状态
static bool wifi_initialized = false;

// Coze OAuth 配置
static const char *coze_authorization_url = "https://api.coze.cn/api/permission/oauth2/token";

// 生成随机字符串
static void generate_random_string(char *output, size_t length)
{
    const char charset[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789";
    size_t charset_size = sizeof(charset) - 1;
    for (size_t i = 0; i < length; i++)
    {
        int key = esp_random() % charset_size;
        output[i] = charset[key];
    }
    output[length] = '\0';
}

// 获取 Coze Access Token（OAuth 流程）
static char *coze_get_access_token(void)
{

    const char *private_key = private_key_pem_start;
    size_t private_key_size = private_key_pem_end - private_key_pem_start;

    ESP_LOGI(TAG, "   - Public Key: %s", COZE_PUBLIC_KEY);
    ESP_LOGI(TAG, "   - Private Key Size: %zu bytes", private_key_size);

    // 构建 JWT payload
    cJSON *payload_json = cJSON_CreateObject();
    if (!payload_json)
    {
        ESP_LOGE(TAG, "❌ 创建 payload_json 失败");
        return NULL;
    }

    char random_str[33] = {0};
    generate_random_string(random_str, 32);
    time_t now = time(NULL);

    cJSON_AddStringToObject(payload_json, "iss", COZE_APP_ID);
    cJSON_AddStringToObject(payload_json, "aud", "api.coze.cn");
    cJSON_AddNumberToObject(payload_json, "iat", now);
    cJSON_AddNumberToObject(payload_json, "exp", now + 6000);
    cJSON_AddStringToObject(payload_json, "jti", random_str);
    cJSON_AddStringToObject(payload_json, "session_name", "esp32_replica_ai_session");

    // session_context
    cJSON *session_context_json = cJSON_CreateObject();
    cJSON *device_info_json = cJSON_CreateObject();
    cJSON_AddStringToObject(device_info_json, "device_id", g_device_id);
    cJSON_AddStringToObject(device_info_json, "custom_consumer", "esp32_replica");
    cJSON_AddItemToObject(session_context_json, "device_info", device_info_json);
    cJSON_AddItemToObject(payload_json, "session_context", session_context_json);

    char *payload_str = cJSON_PrintUnformatted(payload_json);
    if (!payload_str)
    {
        ESP_LOGE(TAG, "❌ 打印 payload_json 失败");
        cJSON_Delete(payload_json);
        return NULL;
    }
    ESP_LOGI(TAG, "   - JWT Payload: %s", payload_str);

    // 生成 JWT
    char *jwt = coze_jwt_create_handler(COZE_PUBLIC_KEY, payload_str,
                                        (const uint8_t *)private_key, private_key_size);
    cJSON_Delete(payload_json);
    free(payload_str);

    if (!jwt)
    {
        ESP_LOGE(TAG, "❌ 创建 JWT 失败");
        return NULL;
    }
    ESP_LOGI(TAG, "   - JWT: %s", jwt);

    // 构建 Authorization header
    char *authorization = (char *)calloc(1, strlen(jwt) + 16);
    if (!authorization)
    {
        ESP_LOGE(TAG, "❌ 分配 authorization 内存失败");
        free(jwt);
        return NULL;
    }
    sprintf(authorization, "Bearer %s", jwt);
    ESP_LOGI(TAG, "   - Authorization: %s", authorization);

    // 构建 HTTP 请求体
    cJSON *http_req_json = cJSON_CreateObject();
    if (!http_req_json)
    {
        ESP_LOGE(TAG, "❌ 创建 http_req_json 失败");
        free(jwt);
        free(authorization);
        return NULL;
    }
    cJSON_AddNumberToObject(http_req_json, "duration_seconds", 86399);
    cJSON_AddStringToObject(http_req_json, "grant_type", "urn:ietf:params:oauth:grant-type:jwt-bearer");
    char *http_req_json_str = cJSON_PrintUnformatted(http_req_json);
    if (!http_req_json_str)
    {
        ESP_LOGE(TAG, "❌ 打印 http_req_json 失败");
        free(jwt);
        free(authorization);
        cJSON_Delete(http_req_json);
        return NULL;
    }
    ESP_LOGI(TAG, "   - HTTP Request: %s", http_req_json_str);

    // 构建 HTTP 请求头
    http_req_header_t header[] = {
        {"Content-Type", "application/json"},
        {"Authorization", authorization},
        {NULL, NULL}};

    // 发送 HTTP POST 请求
    http_response_t response = {0};
    esp_err_t ret = http_client_post(coze_authorization_url, header, http_req_json_str, &response);
    if (ret != ESP_OK)
    {
        ESP_LOGE(TAG, "❌ HTTP POST 失败: %s", esp_err_to_name(ret));
        free(jwt);
        free(authorization);
        cJSON_Delete(http_req_json);
        free(http_req_json_str);
        return NULL;
    }

    // 解析响应，提取 access_token
    char *access_token = NULL;
    if (response.body)
    {
        ESP_LOGI(TAG, "   - HTTP Response: %s", response.body);

        cJSON *root = cJSON_Parse(response.body);
        if (root)
        {
            cJSON *access_token_item = cJSON_GetObjectItem(root, "access_token");
            if (cJSON_IsString(access_token_item) && access_token_item->valuestring != NULL)
            {
                ESP_LOGI(TAG, "✅ 成功获取 Access Token");
                ESP_LOGI(TAG, "   - Access Token: %s", access_token_item->valuestring);
                access_token = strdup(access_token_item->valuestring);

                cJSON *expires_in_item = cJSON_GetObjectItem(root, "expires_in");
                if (cJSON_IsNumber(expires_in_item))
                {
                    ESP_LOGI(TAG, "   - Token 过期时间: %d 秒", expires_in_item->valueint);
                }

                cJSON *token_type_item = cJSON_GetObjectItem(root, "token_type");
                if (cJSON_IsString(token_type_item))
                {
                    ESP_LOGI(TAG, "   - Token 类型: %s", token_type_item->valuestring);
                }
            }
            else
            {
                ESP_LOGE(TAG, "❌ access_token 无效或不存在");
            }

            cJSON_Delete(root);
        }
        else
        {
            ESP_LOGE(TAG, "❌ 解析 JSON 响应失败");
        }
    }

    // 释放内存
    free(jwt);
    free(authorization);
    cJSON_Delete(http_req_json);
    free(http_req_json_str);
    if (response.body)
    {
        free(response.body);
    }

    return access_token;
}

// 事件回调函数
static void coze_event_callback(esp_coze_chat_event_t event, char *data, void *ctx)
{
    switch (event)
    {
    case ESP_COZE_CHAT_EVENT_CHAT_CREATE:
        ESP_LOGI(TAG, "🤖 Coze 会话已创建");
        if (g_ai_chat.event_cb)
        {
            g_ai_chat.event_cb(AI_CHAT_EVENT_CHAT_STARTED, data, g_ai_chat.user_ctx);
        }
        break;

    case ESP_COZE_CHAT_EVENT_CHAT_UPDATE:
        ESP_LOGI(TAG, "📝 Coze 会话更新: %s", data ? data : "null");
        break;

    case ESP_COZE_CHAT_EVENT_CHAT_COMPLETED:
        ESP_LOGI(TAG, "✅ Coze 会话完成");
        g_ai_chat.speaking = false;
        if (g_ai_chat.event_cb)
        {
            g_ai_chat.event_cb(AI_CHAT_EVENT_CHAT_COMPLETED, data, g_ai_chat.user_ctx);
        }
        break;

    case ESP_COZE_CHAT_EVENT_CHAT_SPEECH_STARTED:
        ESP_LOGI(TAG, "🔊 AI 开始说话");
        g_ai_chat.speaking = true;
        if (g_ai_chat.event_cb)
        {
            g_ai_chat.event_cb(AI_CHAT_EVENT_SPEECH_STARTED, NULL, g_ai_chat.user_ctx);
        }
        break;

    case ESP_COZE_CHAT_EVENT_CHAT_SPEECH_STOPED:
        ESP_LOGI(TAG, "🔇 AI 停止说话");
        g_ai_chat.speaking = false;
        if (g_ai_chat.event_cb)
        {
            g_ai_chat.event_cb(AI_CHAT_EVENT_SPEECH_STOPPED, NULL, g_ai_chat.user_ctx);
        }
        break;

    case ESP_COZE_CHAT_EVENT_CHAT_ERROR:
        ESP_LOGE(TAG, "❌ Coze 错误: %s", data ? data : "unknown");
        if (g_ai_chat.event_cb)
        {
            g_ai_chat.event_cb(AI_CHAT_EVENT_ERROR, data, g_ai_chat.user_ctx);
        }
        break;

    case ESP_COZE_CHAT_EVENT_INPUT_AUDIO_BUFFER_COMPLETED:
        ESP_LOGI(TAG, "🎤 输入音频缓冲区处理完成");
        break;

    case ESP_COZE_CHAT_EVENT_CHAT_SUBTITLE_EVENT:
        ESP_LOGI(TAG, "📝 字幕: %s", data ? data : "");
        if (g_ai_chat.event_cb && data)
        {
            g_ai_chat.event_cb(AI_CHAT_EVENT_SUBTITLE, data, g_ai_chat.user_ctx);
        }
        break;

    case ESP_COZE_CHAT_EVENT_CHAT_CUSTOMER_DATA:
        ESP_LOGI(TAG, "📦 自定义数据: %s", data ? data : "null");
        break;

    default:
        ESP_LOGW(TAG, "⚠️  未知事件: %d", event);
        break;
    }
}

// 音频数据回调（接收 AI 返回的语音）
static void audio_data_callback(char *data, int len, void *ctx)
{
    ESP_LOGI(TAG, "🔊 接收到音频数据: %d 字节", len);

    // TODO: 将音频数据送入播放器
    // audio_playback_feed_data((uint8_t *)data, len);

    // 这里暂时只打印日志
    ESP_LOGI(TAG, "🔊 音频数据已接收，需要实现音频播放器");
}

// 音频读取任务
static void audio_read_task(void *pvParameters)
{
    ESP_LOGI(TAG, "🎤 音频读取任务已启动");

    const int buffer_size = 1024;
    uint8_t *buffer = (uint8_t *)malloc(buffer_size);
    if (!buffer)
    {
        ESP_LOGE(TAG, "❌ 音频缓冲区分配失败");
        vTaskDelete(NULL);
        return;
    }

    while (g_ai_chat.started && !g_ai_chat.paused && g_ai_chat.wakeup)
    {
        // TODO: 从麦克风读取音频数据
        // int read_size = audio_recorder_read_data(buffer, buffer_size);

        // 暂时使用模拟数据
        // ESP_LOGI(TAG, "🎤 读取音频数据: %d 字节", read_size);

        // 如果处于唤醒状态，发送音频到 Coze
        // if (read_size > 0 && g_ai_chat.wakeup && !g_ai_chat.speaking && !g_ai_chat.sleeping) {
        //     esp_err_t ret = esp_coze_chat_send_audio_data(g_ai_chat.chat_handle, (char *)buffer, read_size);
        //     if (ret != ESP_OK) {
        //         ESP_LOGE(TAG, "❌ 发送音频数据失败: %s", esp_err_to_name(ret));
        //     }
        // }

        vTaskDelay(pdMS_TO_TICKS(20)); // 20ms 周期
    }

    free(buffer);
    ESP_LOGI(TAG, "🎤 音频读取任务已停止");
    vTaskDelete(NULL);
}

esp_err_t ai_chat_init(ai_chat_event_callback_t event_cb, void *ctx)
{
    ESP_LOGI(TAG, "");
    ESP_LOGI(TAG, "╔═════════════════════════════════════════╗");
    ESP_LOGI(TAG, "║      初始化 AI 对话系统                     ║");
    ESP_LOGI(TAG, "╚═════════════════════════════════════════╝");
    ESP_LOGI(TAG, "");

    if (g_ai_chat.initialized)
    {
        ESP_LOGW(TAG, "⚠️  AI 对话系统已初始化");
        return ESP_OK;
    }

    g_ai_chat.event_cb = event_cb;
    g_ai_chat.user_ctx = ctx;

    ESP_LOGI(TAG, "📋 配置信息：");
    ESP_LOGI(TAG, "   - App ID: %s", COZE_APP_ID);
    ESP_LOGI(TAG, "   - Bot ID: %s", COZE_BOT_ID);
    ESP_LOGI(TAG, "   - Bot Name: %s", COZE_BOT_NAME);
    ESP_LOGI(TAG, "   - Voice ID: %s", COZE_VOICE_ID);
    ESP_LOGI(TAG, "   - Public Key: %s", COZE_PUBLIC_KEY);
    ESP_LOGI(TAG, "");

    // 生成设备 ID
    snprintf(g_device_id, sizeof(g_device_id), "device_001");

    ESP_LOGI(TAG, "✅ AI 对话系统初始化完成（将在 WiFi 连接后获取 Token）");

    g_ai_chat.initialized = true;

    ESP_LOGI(TAG, "");
    ESP_LOGI(TAG, "╔═══════════════════════════════════════════╗");
    ESP_LOGI(TAG, "║      AI 对话系统初始化完成                    ║");
    ESP_LOGI(TAG, "╚═════════════════════════════════════════╝");
    ESP_LOGI(TAG, "");

    return ESP_OK;
}

esp_err_t ai_chat_deinit(void)
{
    ESP_LOGI(TAG, "🛑 反初始化 AI 对话系统...");

    if (!g_ai_chat.initialized)
    {
        ESP_LOGW(TAG, "⚠️  AI 对话系统未初始化");
        return ESP_OK;
    }

    // 停止 Coze 聊天会话
    if (g_ai_chat.chat_handle)
    {
        esp_err_t ret = esp_coze_chat_deinit(g_ai_chat.chat_handle);
        if (ret != ESP_OK)
        {
            ESP_LOGE(TAG, "❌ Coze 聊天会话反初始化失败: %s", esp_err_to_name(ret));
        }
        g_ai_chat.chat_handle = NULL;
    }

    g_ai_chat.initialized = false;
    g_ai_chat.started = false;

    ESP_LOGI(TAG, "✅ AI 对话系统已反初始化");
    return ESP_OK;
}

esp_err_t ai_chat_start(void)
{
    ESP_LOGI(TAG, "▶️  启动 AI 对话...");

    if (!g_ai_chat.initialized)
    {
        ESP_LOGE(TAG, "❌ AI 对话系统未初始化");
        return ESP_ERR_INVALID_STATE;
    }

    if (g_ai_chat.started)
    {
        ESP_LOGW(TAG, "⚠️  AI 对话已启动");
        return ESP_OK;
    }

    esp_err_t ret;

    // 初始化 WiFi 连接（如果尚未初始化）
    if (!wifi_initialized)
    {
        ESP_LOGI(TAG, "");
        ESP_LOGI(TAG, "═══════════════════════════════════════════");
        ESP_LOGI(TAG, "📶 开始初始化 WiFi 连接...");
        ESP_LOGI(TAG, "═══════════════════════════════════════════");
        ESP_LOGI(TAG, "");

        ret = wifi_connect_init();
        if (ret != ESP_OK)
        {
            ESP_LOGE(TAG, "❌ WiFi 初始化失败: %s", esp_err_to_name(ret));
            return ret;
        }

        // 等待 WiFi 连接完成
        ret = wifi_connect_wait(30000); // 30 秒超时
        if (ret != ESP_OK)
        {
            ESP_LOGE(TAG, "❌ WiFi 连接失败: %s", esp_err_to_name(ret));
            return ret;
        }

        wifi_initialized = true;
        ESP_LOGI(TAG, "");
        ESP_LOGI(TAG, "✅ WiFi 连接已就绪，可以启动 AI 对话");
        ESP_LOGI(TAG, "");
    }

    // 初始化 SNTP（WiFi 和 TCP/IP 已经初始化后）
    ESP_LOGI(TAG, "🕐 初始化 SNTP...");
    ret = wifi_sync_time_init();
    if (ret != ESP_OK)
    {
        ESP_LOGE(TAG, "❌ SNTP 初始化失败: %s", esp_err_to_name(ret));
        return ret;
    }
    ESP_LOGI(TAG, "✅ SNTP 初始化完成");

    // 启动 SNTP 时间同步（WiFi 连接后必须执行）
    ESP_LOGI(TAG, "🕐 启动 SNTP 时间同步...");
    ret = wifi_sync_time_start(60000); // 60 秒超时（30次 * 2秒）
    if (ret != ESP_OK)
    {
        ESP_LOGE(TAG, "❌ 时间同步失败: %s", esp_err_to_name(ret));
        return ret;
    }
    ESP_LOGI(TAG, "✅ 时间同步完成");


    // 获取 Access Token（OAuth 流程）- WiFi 连接后执行
    ESP_LOGI(TAG, "🔑 开始获取 Coze Access Token...");
    char *token = coze_get_access_token();
    if (!token)
    {
        ESP_LOGE(TAG, "❌ 获取 Access Token 失败");
        return ESP_FAIL;
    }
    snprintf(g_access_token, sizeof(g_access_token), "%s", token);
    free(token);
    ESP_LOGI(TAG, "✅ Access Token 获取成功");

    // 配置 Coze 聊天会话
    ESP_LOGI(TAG, "🤖 初始化 Coze 聊天会话...");
    esp_coze_chat_config_t chat_config = ESP_COZE_CHAT_DEFAULT_CONFIG();
    chat_config.bot_id = (char *)COZE_BOT_ID;
    chat_config.voice_id = (char *)COZE_VOICE_ID;
    chat_config.user_id = g_device_id;
    chat_config.access_token = g_access_token;
    chat_config.enable_subtitle = true;

    // 设置回调
    chat_config.event_callback = coze_event_callback;
    chat_config.audio_callback = audio_data_callback;

    // 创建 Coze 聊天会话
    ret = esp_coze_chat_init(&chat_config, &g_ai_chat.chat_handle);
    if (ret != ESP_OK)
    {
        ESP_LOGE(TAG, "❌ Coze 聊天会话初始化失败: %s", esp_err_to_name(ret));
        return ret;
    }
    ESP_LOGI(TAG, "✅ Coze 聊天会话初始化成功");

    // 创建音频读取任务
    ESP_LOGI(TAG, "🎤 创建音频读取任务...");
    BaseType_t task_ret = xTaskCreatePinnedToCore(
        audio_read_task,
        "audio_read",
        4096,
        NULL,
        (tskIDLE_PRIORITY + 2),
        &g_ai_chat.audio_read_task,
        tskNO_AFFINITY);

    if (task_ret != pdPASS)
    {
        ESP_LOGE(TAG, "❌ 音频读取任务创建失败");
        esp_coze_chat_deinit(g_ai_chat.chat_handle);
        g_ai_chat.chat_handle = NULL;
        return ESP_FAIL;
    }
    ESP_LOGI(TAG, "✅ 音频读取任务创建成功");

    // 启动 Coze 聊天会话
    ESP_LOGI(TAG, "🤖 启动 Coze 聊天会话...");
    ret = esp_coze_chat_start(g_ai_chat.chat_handle);
    if (ret != ESP_OK)
    {
        ESP_LOGE(TAG, "❌ Coze 聊天会话启动失败: %s", esp_err_to_name(ret));
        return ret;
    }

    g_ai_chat.started = true;
    g_ai_chat.paused = false;
    g_ai_chat.sleeping = false;

    ESP_LOGI(TAG, "✅ AI 对话已启动");
    return ESP_OK;
}

esp_err_t ai_chat_stop(void)
{
    ESP_LOGI(TAG, "⏹️  停止 AI 对话...");

    if (!g_ai_chat.initialized)
    {
        ESP_LOGE(TAG, "❌ AI 对话系统未初始化");
        return ESP_ERR_INVALID_STATE;
    }

    if (!g_ai_chat.started)
    {
        ESP_LOGW(TAG, "⚠️  AI 对话未启动");
        return ESP_OK;
    }

    // 停止 Coze 聊天会话
    esp_err_t ret = esp_coze_chat_stop(g_ai_chat.chat_handle);
    if (ret != ESP_OK)
    {
        ESP_LOGE(TAG, "❌ Coze 聊天会话停止失败: %s", esp_err_to_name(ret));
        return ret;
    }

    g_ai_chat.started = false;

    ESP_LOGI(TAG, "✅ AI 对话已停止");
    return ESP_OK;
}

esp_err_t ai_chat_pause(void)
{
    ESP_LOGI(TAG, "⏸️  暂停 AI 对话...");

    if (!g_ai_chat.started)
    {
        ESP_LOGW(TAG, "⚠️  AI 对话未启动");
        return ESP_OK;
    }

    g_ai_chat.paused = true;

    ESP_LOGI(TAG, "✅ AI 对话已暂停");
    return ESP_OK;
}

esp_err_t ai_chat_resume(void)
{
    ESP_LOGI(TAG, "▶️  恢复 AI 对话...");

    if (!g_ai_chat.started)
    {
        ESP_LOGW(TAG, "⚠️  AI 对话未启动");
        return ESP_OK;
    }

    g_ai_chat.paused = false;

    ESP_LOGI(TAG, "✅ AI 对话已恢复");
    return ESP_OK;
}

esp_err_t ai_chat_wakeup(void)
{
    ESP_LOGI(TAG, "🎤 唤醒 AI...");

    if (!g_ai_chat.started || g_ai_chat.paused)
    {
        ESP_LOGW(TAG, "⚠️  AI 对话未启动或已暂停");
        return ESP_ERR_INVALID_STATE;
    }

    if (g_ai_chat.wakeup)
    {
        ESP_LOGW(TAG, "⚠️  AI 已唤醒");
        return ESP_OK;
    }

    g_ai_chat.wakeup = true;
    g_ai_chat.sleeping = false;

    // 通知回调
    if (g_ai_chat.event_cb)
    {
        g_ai_chat.event_cb(AI_CHAT_EVENT_WAKE_UP, NULL, g_ai_chat.user_ctx);
    }

    ESP_LOGI(TAG, "✅ AI 已唤醒，可以开始对话");
    return ESP_OK;
}

esp_err_t ai_chat_sleep(void)
{
    ESP_LOGI(TAG, "😴 让 AI 进入睡眠模式...");

    if (!g_ai_chat.started)
    {
        ESP_LOGW(TAG, "⚠️  AI 对话未启动");
        return ESP_ERR_INVALID_STATE;
    }

    g_ai_chat.wakeup = false;
    g_ai_chat.sleeping = true;
    g_ai_chat.speaking = false;

    // 先中断当前对话
    ai_chat_interrupt();

    // 通知回调
    if (g_ai_chat.event_cb)
    {
        g_ai_chat.event_cb(AI_CHAT_EVENT_SLEEP, NULL, g_ai_chat.user_ctx);
    }

    ESP_LOGI(TAG, "✅ AI 已进入睡眠模式");
    return ESP_OK;
}

esp_err_t ai_chat_interrupt(void)
{
    ESP_LOGI(TAG, "🛑 中断当前对话...");

    if (!g_ai_chat.started)
    {
        ESP_LOGW(TAG, "⚠️  AI 对话未启动");
        return ESP_ERR_INVALID_STATE;
    }

    // 中断 Coze 聊天会话
    esp_err_t ret = esp_coze_chat_send_audio_cancel(g_ai_chat.chat_handle);
    if (ret != ESP_OK)
    {
        ESP_LOGE(TAG, "❌ 中断对话失败: %s", esp_err_to_name(ret));
        return ret;
    }

    ESP_LOGI(TAG, "✅ 对话已中断");
    return ESP_OK;
}
