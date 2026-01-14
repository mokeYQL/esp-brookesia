/*
 * SPDX-FileCopyrightText: 2025 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: CC0-1.0
 */

#include "nvs_storage.h"
#include "esp_log.h"
#include "nvs_flash.h"
#include "nvs.h"

static const char *TAG = "NVS_STORAGE";

#define NVS_DEFAULT_NAMESPACE "storage"

esp_err_t nvs_storage_init(void)
{
    ESP_LOGI(TAG, "");
    ESP_LOGI(TAG, "╔═════════════════════════════════════════╗");
    ESP_LOGI(TAG, "║      初始化 NVS 存储系统                    ║");
    ESP_LOGI(TAG, "╚═════════════════════════════════════════╝");
    ESP_LOGI(TAG, "");

    // 初始化 NVS Flash
    ESP_LOGI(TAG, "💾 初始化 NVS Flash...");
    esp_err_t ret = nvs_flash_init();

    // 处理 NVS 初始化错误
    if (ret == ESP_ERR_NVS_NO_FREE_PAGES) {
        ESP_LOGW(TAG, "⚠️  NVS 分区已满，正在擦除...");
        ret = nvs_flash_erase();
        if (ret != ESP_OK) {
            ESP_LOGE(TAG, "❌ 擦除 NVS Flash 失败: %s", esp_err_to_name(ret));
            return ret;
        }
        ESP_LOGI(TAG, "✅ NVS Flash 擦除成功");

        // 重新初始化
        ESP_LOGI(TAG, "🔄 重新初始化 NVS Flash...");
        ret = nvs_flash_init();
        if (ret != ESP_OK) {
            ESP_LOGE(TAG, "❌ 重新初始化 NVS Flash 失败: %s", esp_err_to_name(ret));
            return ret;
        }
        ESP_LOGI(TAG, "✅ NVS Flash 重新初始化成功");
    } else if (ret == ESP_ERR_NVS_NEW_VERSION_FOUND) {
        ESP_LOGW(TAG, "⚠️  检测到 NVS 版本不匹配，正在擦除...");
        ret = nvs_flash_erase();
        if (ret != ESP_OK) {
            ESP_LOGE(TAG, "❌ 擦除 NVS Flash 失败: %s", esp_err_to_name(ret));
            return ret;
        }
        ESP_LOGI(TAG, "✅ NVS Flash 擦除成功");

        // 重新初始化
        ESP_LOGI(TAG, "🔄 重新初始化 NVS Flash...");
        ret = nvs_flash_init();
        if (ret != ESP_OK) {
            ESP_LOGE(TAG, "❌ 重新初始化 NVS Flash 失败: %s", esp_err_to_name(ret));
            return ret;
        }
        ESP_LOGI(TAG, "✅ NVS Flash 重新初始化成功");
    } else if (ret != ESP_OK) {
        ESP_LOGE(TAG, "❌ 初始化 NVS Flash 失败: %s", esp_err_to_name(ret));
        return ret;
    }

    ESP_LOGI(TAG, "✅ NVS Flash 初始化成功");

    // 创建并打开默认命名空间（可选）
    ESP_LOGI(TAG, "📂 创建 NVS 命名空间...");
    nvs_handle_t nvs_handle;
    ret = nvs_open(NVS_DEFAULT_NAMESPACE, NVS_READWRITE, &nvs_handle);
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "❌ 打开 NVS 命名空间失败: %s", esp_err_to_name(ret));
        return ret;
    }

    // 提交更改
    ret = nvs_commit(nvs_handle);
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "❌ 提交 NVS 失败: %s", esp_err_to_name(ret));
        nvs_close(nvs_handle);
        return ret;
    }

    // 关闭句柄
    nvs_close(nvs_handle);
    ESP_LOGI(TAG, "✅ NVS 命名空间 '%s' 创建成功", NVS_DEFAULT_NAMESPACE);

    ESP_LOGI(TAG, "");
    ESP_LOGI(TAG, "╔═════════════════════════════════════════╗");
    ESP_LOGI(TAG, "║      NVS 存储系统初始化完成                  ║");
    ESP_LOGI(TAG, "╚═════════════════════════════════════════╝");
    ESP_LOGI(TAG, "");

    return ESP_OK;
}

esp_err_t nvs_storage_deinit(void)
{
    ESP_LOGI(TAG, "🛑 反初始化 NVS 存储系统...");

    esp_err_t ret = nvs_flash_deinit();
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "❌ 反初始化 NVS Flash 失败: %s", esp_err_to_name(ret));
        return ret;
    }

    ESP_LOGI(TAG, "✅ NVS 存储系统已反初始化");
    return ESP_OK;
}
