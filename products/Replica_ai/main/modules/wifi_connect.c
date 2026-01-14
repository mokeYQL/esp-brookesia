/*
 * SPDX-FileCopyrightText: 2025 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: CC0-1.0
 */

#include <string.h>
#include <time.h>
#include "wifi_connect.h"
#include "nvs_storage.h"
#include "esp_log.h"
#include "esp_wifi.h"
#include "esp_event.h"
#include "esp_netif.h"
#include "esp_sntp.h"
#include "esp_netif_sntp.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/event_groups.h"

static const char *TAG = "WIFI_CONNECT";

// WiFi 连接事件标志位
#define WIFI_CONNECTED_BIT BIT0
#define WIFI_FAIL_BIT      BIT1

// 事件组句柄
static EventGroupHandle_t s_wifi_event_group;

// WiFi 连接状态
static bool wifi_initialized = false;
static bool nvs_initialized = false;

// WiFi 连接重试次数
static int s_retry_num = 0;
#define MAX_RETRY 5

// WiFi 事件处理程序
static void wifi_event_handler(void *arg, esp_event_base_t event_base,
                               int32_t event_id, void *event_data)
{
    if (event_base == WIFI_EVENT && event_id == WIFI_EVENT_STA_START) {
        esp_wifi_connect();
        ESP_LOGI(TAG, "WiFi 正在连接...");
    } else if (event_base == WIFI_EVENT && event_id == WIFI_EVENT_STA_DISCONNECTED) {
        if (s_retry_num < MAX_RETRY) {
            esp_wifi_connect();
            s_retry_num++;
            ESP_LOGI(TAG, "WiFi 连接断开，正在重试 (%d/%d)...", s_retry_num, MAX_RETRY);
        } else {
            xEventGroupSetBits(s_wifi_event_group, WIFI_FAIL_BIT);
            ESP_LOGE(TAG, "WiFi 连接失败，已达到最大重试次数");
        }
    } else if (event_base == IP_EVENT && event_id == IP_EVENT_STA_GOT_IP) {
        ip_event_got_ip_t *event = (ip_event_got_ip_t *)event_data;
        ESP_LOGI(TAG, "📶 WiFi 已连接！IP 地址: " IPSTR, IP2STR(&event->ip_info.ip));
        s_retry_num = 0;
        xEventGroupSetBits(s_wifi_event_group, WIFI_CONNECTED_BIT);
    }
}

esp_err_t wifi_connect_init(void)
{
    ESP_LOGI(TAG, "");
    ESP_LOGI(TAG, "╔═════════════════════════════════════════╗");
    ESP_LOGI(TAG, "║      初始化 WiFi 连接系统                   ║");
    ESP_LOGI(TAG, "╚═════════════════════════════════════════╝");
    ESP_LOGI(TAG, "");

    // 初始化 NVS（必须先于 WiFi）
    if (!nvs_initialized) {
        ESP_LOGI(TAG, "");
        ESP_LOGI(TAG, "═══════════════════════════════════════════");
        ESP_LOGI(TAG, "💾 开始初始化 NVS 存储系统...");
        ESP_LOGI(TAG, "═══════════════════════════════════════════");
        ESP_LOGI(TAG, "");

        esp_err_t ret = nvs_storage_init();
        if (ret != ESP_OK) {
            ESP_LOGE(TAG, "❌ NVS 初始化失败: %s", esp_err_to_name(ret));
            return ret;
        }

        nvs_initialized = true;
        ESP_LOGI(TAG, "✅ NVS 初始化完成");
        ESP_LOGI(TAG, "");
    }

    // 创建事件组
    s_wifi_event_group = xEventGroupCreate();
    if (s_wifi_event_group == NULL) {
        ESP_LOGE(TAG, "❌ 创建 WiFi 事件组失败");
        return ESP_FAIL;
    }

    // 初始化 TCP/IP 网络接口（必须先于 WiFi）
    ESP_LOGI(TAG, "🌐 初始化 TCP/IP 网络接口...");
    esp_err_t ret = esp_netif_init();
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "❌ TCP/IP 网络接口初始化失败: %s", esp_err_to_name(ret));
        return ret;
    }
    ESP_LOGI(TAG, "✅ TCP/IP 网络接口初始化成功");

    // 创建默认事件循环
    ESP_LOGI(TAG, "🔄 创建默认事件循环...");
    ret = esp_event_loop_create_default();
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "❌ 创建默认事件循环失败: %s", esp_err_to_name(ret));
        return ret;
    }
    ESP_LOGI(TAG, "✅ 默认事件循环创建成功");

    // 创建 WiFi 站接口
    ESP_LOGI(TAG, "📶 创建 WiFi 站接口...");
    esp_netif_t *sta_netif = esp_netif_create_default_wifi_sta();
    if (sta_netif == NULL) {
        ESP_LOGE(TAG, "❌ 创建 WiFi 站接口失败");
        return ESP_FAIL;
    }
    ESP_LOGI(TAG, "✅ WiFi 站接口创建成功");

    // 初始化 WiFi
    ESP_LOGI(TAG, "📶 初始化 WiFi...");
    wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
    ret = esp_wifi_init(&cfg);
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "❌ WiFi 初始化失败: %s", esp_err_to_name(ret));
        return ret;
    }
    ESP_LOGI(TAG, "✅ WiFi 初始化成功");

    // 注册事件处理程序
    ret = esp_event_handler_instance_register(WIFI_EVENT,
                                            ESP_EVENT_ANY_ID,
                                            &wifi_event_handler,
                                            NULL,
                                            NULL);
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "❌ 注册 WiFi 事件处理程序失败: %s", esp_err_to_name(ret));
        return ret;
    }

    ret = esp_event_handler_instance_register(IP_EVENT,
                                            IP_EVENT_STA_GOT_IP,
                                            &wifi_event_handler,
                                            NULL,
                                            NULL);
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "❌ 注册 IP 事件处理程序失败: %s", esp_err_to_name(ret));
        return ret;
    }
    ESP_LOGI(TAG, "✅ 事件处理程序注册成功");

    // 设置 WiFi 模式为站模式
    ESP_LOGI(TAG, "📶 设置 WiFi 模式为 STA...");
    ret = esp_wifi_set_mode(WIFI_MODE_STA);
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "❌ 设置 WiFi 模式失败: %s", esp_err_to_name(ret));
        return ret;
    }

    // 配置 WiFi
    ESP_LOGI(TAG, "🔧 配置 WiFi 参数...");
    wifi_config_t wifi_config = {
        .sta = {
            .ssid = "yql",
            .password = "12345678",
            .scan_method = WIFI_FAST_SCAN,
            .bssid_set = false,
            .channel = 0,
            .listen_interval = 3,
            .sort_method = WIFI_CONNECT_AP_BY_SIGNAL,
            .threshold = {
                .rssi = -127,
                .authmode = WIFI_AUTH_WPA2_PSK,
                .rssi_5g_adjustment = 0},
            .pmf_cfg = {.capable = true, .required = false},
        },
    };
    ret = esp_wifi_set_config(WIFI_IF_STA, &wifi_config);
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "❌ 设置 WiFi 配置失败: %s", esp_err_to_name(ret));
        return ret;
    }
    ESP_LOGI(TAG, "   - SSID: yql");
    ESP_LOGI(TAG, "   - Password: 12345678");
    ESP_LOGI(TAG, "✅ WiFi 配置成功");

    // 启动 WiFi
    ESP_LOGI(TAG, "▶️  启动 WiFi...");
    ret = esp_wifi_start();
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "❌ 启动 WiFi 失败: %s", esp_err_to_name(ret));
        return ret;
    }
    ESP_LOGI(TAG, "✅ WiFi 已启动");

    ESP_LOGI(TAG, "");
    ESP_LOGI(TAG, "╔═════════════════════════════════════════╗");
    ESP_LOGI(TAG, "║      WiFi 连接系统初始化完成                 ║");
    ESP_LOGI(TAG, "╚═════════════════════════════════════════╝");
    ESP_LOGI(TAG, "");

    return ESP_OK;
}

esp_err_t wifi_connect_wait(uint32_t timeout_ms)
{
    ESP_LOGI(TAG, "⏳ 等待 WiFi 连接完成（超时: %d ms）...", timeout_ms);

    // 等待连接完成
    EventBits_t bits = xEventGroupWaitBits(s_wifi_event_group,
                                           WIFI_CONNECTED_BIT | WIFI_FAIL_BIT,
                                           pdFALSE,
                                           pdFALSE,
                                           pdMS_TO_TICKS(timeout_ms));

    if (bits & WIFI_CONNECTED_BIT) {
        ESP_LOGI(TAG, "✅ WiFi 连接成功！");
        return ESP_OK;
    } else if (bits & WIFI_FAIL_BIT) {
        ESP_LOGE(TAG, "❌ WiFi 连接失败");
        return ESP_FAIL;
    } else {
        ESP_LOGE(TAG, "❌ WiFi 连接超时");
        return ESP_ERR_TIMEOUT;
    }
}

// NTP 同步回调函数
static void sntp_sync_callback(struct timeval *tv)
{
    ESP_LOGI(TAG, "🕐 SNTP 同步回调被触发！时间戳: %ld", tv->tv_sec);
}

static bool sntp_initialized = false;

esp_err_t wifi_sync_time_init(void)
{
    if (sntp_initialized) {
        return ESP_OK;
    }

    ESP_LOGI(TAG, "");
    ESP_LOGI(TAG, "═══════════════════════════════════════════");
    ESP_LOGI(TAG, "🕐 初始化 SNTP 时间同步系统...");
    ESP_LOGI(TAG, "═══════════════════════════════════════════");
    ESP_LOGI(TAG, "");

    // 初始化 NTP - 不启动，等 WiFi 连接后再启动
    ESP_LOGI(TAG, "🌐 初始化 NTP 服务（使用阿里云 NTP 服务器）");
    esp_sntp_config_t sntp_config = {
        .smooth_sync = false,
        .server_from_dhcp = false,
        .wait_for_sync = false,
        .start = false,  // 不自动启动，等待显式启动
        .sync_cb = sntp_sync_callback,
        .renew_servers_after_new_IP = false,
        .ip_event_to_renew = IP_EVENT_STA_GOT_IP,
        .index_of_first_server = 0,
        .num_of_servers = 1,
        .servers = {"ntp.aliyun.com"}};

    esp_err_t ret = esp_netif_sntp_init(&sntp_config);
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "❌ NTP 初始化失败: %s", esp_err_to_name(ret));
        return ret;
    }

    sntp_initialized = true;
    ESP_LOGI(TAG, "✅ SNTP 初始化成功");
    ESP_LOGI(TAG, "");

    return ESP_OK;
}

esp_err_t wifi_sync_time_start(uint32_t timeout_ms)
{
    ESP_LOGI(TAG, "");
    ESP_LOGI(TAG, "═══════════════════════════════════════════");
    ESP_LOGI(TAG, "🕐 启动 SNTP 时间同步...");
    ESP_LOGI(TAG, "═══════════════════════════════════════════");
    ESP_LOGI(TAG, "");

    // 启动 SNTP 服务
    ESP_LOGI(TAG, "🚀 启动 SNTP 服务...");
    esp_err_t ret = esp_netif_sntp_start();
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "❌ SNTP 启动失败: %s", esp_err_to_name(ret));
        return ret;
    }

    // 等待时间同步完成（使用 esp_netif_sntp_sync_wait）
    ESP_LOGI(TAG, "⏳ 等待时间同步完成...");
    int retry = 0;
    const int retry_count_max = timeout_ms / 2000;  // 每2秒检查一次

    while (esp_netif_sntp_sync_wait(2000 / portTICK_PERIOD_MS) == ESP_ERR_TIMEOUT) {
        retry++;
        if (retry >= retry_count_max) {
            ESP_LOGE(TAG, "❌ 时间同步超时");
            return ESP_ERR_TIMEOUT;
        }
        ESP_LOGI(TAG, "⏳ 等待系统时间同步... (%d/%d)", retry, retry_count_max);
    }

    ESP_LOGI(TAG, "✅ 时间同步完成");

    // 设置时区（中国标准时间，UTC+8）
    ESP_LOGI(TAG, "🕐 设置时区为 CST-8（中国标准时间）");
    setenv("TZ", "CST-8", 1);
    tzset();

    // 打印同步后的时间
    time_t now;
    time(&now);
    struct tm timeinfo;
    localtime_r(&now, &timeinfo);
    char strftime_buf[64];
    strftime(strftime_buf, sizeof(strftime_buf), "%c", &timeinfo);

    ESP_LOGI(TAG, "");
    ESP_LOGI(TAG, "✅ 时间同步成功！");
    ESP_LOGI(TAG, "   - 当前时间: %s", strftime_buf);
    ESP_LOGI(TAG, "   - Unix 时间戳: %ld", (long)now);
    ESP_LOGI(TAG, "");

    ESP_LOGI(TAG, "╔═══════════════════════════════════════════╗");
    ESP_LOGI(TAG, "║      系统时间同步完成                         ║");
    ESP_LOGI(TAG, "╚═══════════════════════════════════════════╝");
    ESP_LOGI(TAG, "");

    return ESP_OK;
}
