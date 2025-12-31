/*
 * SPDX-FileCopyrightText: 2025 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: CC0-1.0
 */

#include <algorithm>
#include <string>

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#include "esp_check.h"
#include "esp_codec_dev.h"
#include "esp_log.h"
#include "esp_spiffs.h"

#include "bsp/esp-bsp.h"
#include "esp_brookesia.hpp"

using namespace esp_brookesia::services;
using namespace esp_brookesia::systems::speaker;
using namespace esp_brookesia::ai_framework;

namespace
{

    constexpr const char *TAG = "AudioMini";
    constexpr const char *PROMPT_URL = "file://spiffs/response_wo_zai_ting_ne.mp3";
    constexpr int SOUND_VOLUME_MIN = 0;
    constexpr int SOUND_VOLUME_MAX = 100;
    constexpr int SOUND_VOLUME_DEFAULT = 50;

    esp_codec_dev_handle_t s_play_dev = nullptr;
    esp_codec_dev_handle_t s_rec_dev = nullptr;

    bool mount_spiffs()
    {
        esp_vfs_spiffs_conf_t conf = {
            .base_path = "/spiffs",
            .partition_label = "spiffs_data",
            .max_files = 5,
            .format_if_mount_failed = false,
        };
        esp_err_t ret = esp_vfs_spiffs_register(&conf);
        if (ret == ESP_ERR_NOT_FOUND)
        {
            ESP_LOGE(TAG, "SPIFFS partition not found");
            return false;
        }
        ESP_ERROR_CHECK(ret);

        size_t total = 0;
        size_t used = 0;
        ESP_ERROR_CHECK(esp_spiffs_info(conf.partition_label, &total, &used));
        ESP_LOGI(TAG, "SPIFFS mounted: total=%d KB, used=%d KB", (int)(total / 1024), (int)(used / 1024));
        return true;
    }

    void apply_volume_from_nvs(StorageNVS &storage_service)
    {
        StorageNVS::Value value;
        if (!storage_service.getLocalParam(Manager::SETTINGS_VOLUME, value))
        {
            ESP_LOGW(TAG, "Volume not found in NVS");
            return;
        }

        int volume = std::clamp(std::get<int>(value), SOUND_VOLUME_MIN, SOUND_VOLUME_MAX);
        ESP_LOGI(TAG, "Apply volume=%d", volume);
        if (s_play_dev != nullptr)
        {
            ESP_ERROR_CHECK(esp_codec_dev_set_out_vol(s_play_dev, volume) == ESP_CODEC_DEV_OK ? ESP_OK : ESP_FAIL);
        }
    }

    bool init_audio_pipeline()
    {
        ESP_ERROR_CHECK(bsp_i2c_init());

        esp_gmf_setup_periph_hardware_info periph_info = {
            .i2c = {.handle = bsp_i2c_get_handle()},
            .codec = {
                .io_pa = BSP_POWER_AMP_IO,
                .type = ESP_GMF_CODEC_TYPE_ES7210_IN_ES8311_OUT,
                .dac = {
                    .io_mclk = BSP_I2S_MCLK,
                    .io_bclk = BSP_I2S_SCLK,
                    .io_ws = BSP_I2S_LCLK,
                    .io_do = BSP_I2S_DOUT,
                    .io_di = BSP_I2S_DSIN,
                    .sample_rate = 16000,
                    .channel = 2,
                    .bits_per_sample = 32,
                    .port_num = 0,
                },
                .adc = {
                    .io_mclk = BSP_I2S_MCLK,
                    .io_bclk = BSP_I2S_SCLK,
                    .io_ws = BSP_I2S_LCLK,
                    .io_do = BSP_I2S_DOUT,
                    .io_di = BSP_I2S_DSIN,
                    .sample_rate = 16000,
                    .channel = 2,
                    .bits_per_sample = 32,
                    .port_num = 0,
                },
            },
        };

        ESP_ERROR_CHECK(audio_manager_init(&periph_info, (void **)&s_play_dev, (void **)&s_rec_dev));
        ESP_ERROR_CHECK(audio_prompt_open());

        auto &storage_service = StorageNVS::requestInstance();
        storage_service.connectEventSignal([&](const StorageNVS::Event &event)
                                           {
        if ((event.operation != StorageNVS::Operation::UpdateNVS) || (event.key != Manager::SETTINGS_VOLUME)) {
            return;
        }
        apply_volume_from_nvs(storage_service); });

        StorageNVS::Value value = SOUND_VOLUME_DEFAULT;
        if (!storage_service.getLocalParam(Manager::SETTINGS_VOLUME, value))
        {
            ESP_LOGW(TAG, "Volume not found, set to default=%d", SOUND_VOLUME_DEFAULT);
        }
        ESP_ERROR_CHECK(storage_service.setLocalParam(Manager::SETTINGS_VOLUME, value) ? ESP_OK : ESP_FAIL);
        apply_volume_from_nvs(storage_service);

        return true;
    }

    void prompt_task(void *)
    {
        while (true)
        {
            ESP_LOGI(TAG, "Play prompt: %s", PROMPT_URL);
            esp_err_t ret = audio_prompt_play_with_block(PROMPT_URL, 5000);
            if (ret != ESP_OK)
            {
                ESP_LOGW(TAG, "Failed to play prompt (%s)", esp_err_to_name(ret));
            }
            vTaskDelay(pdMS_TO_TICKS(10000));
        }
    }

} // namespace

extern "C" void app_main(void)
{
    ESP_LOGI(TAG, "Boot speaker audio mini demo");
    /* Initialize power for peripherals, including audio codec */
    ESP_ERROR_CHECK(bsp_power_init(true));

    auto &storage_service = StorageNVS::requestInstance();
    ESP_ERROR_CHECK(storage_service.begin() ? ESP_OK : ESP_FAIL);
    ESP_ERROR_CHECK(mount_spiffs() ? ESP_OK : ESP_FAIL);
    ESP_ERROR_CHECK(init_audio_pipeline() ? ESP_OK : ESP_FAIL);

    xTaskCreatePinnedToCore(prompt_task, "audio_prompt", 4096, nullptr, 4, nullptr, tskNO_AFFINITY);

    while (true)
    {
        vTaskDelay(pdMS_TO_TICKS(1000));
    }
}
