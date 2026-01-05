/*
 * SPDX-FileCopyrightText: 2025 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: CC0-1.0
 */

#include <algorithm>
#include <string>
#include <time.h>

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#include "esp_check.h"
#include "esp_codec_dev.h"
#include "esp_log.h"
#include "esp_spiffs.h"
#include "esp_wifi.h"
#include "esp_event.h"
#include "esp_netif.h"
#include "esp_sntp.h"
#include "esp_netif_sntp.h"

#include "bsp/esp-bsp.h"
#include "esp_brookesia.hpp"
#include "coze_agent_config.h"
#include "coze_agent_config_default.h"

extern const char private_key_pem_start[] asm("_binary_private_key_pem_start");
extern const char private_key_pem_end[] asm("_binary_private_key_pem_end");

using namespace esp_brookesia::services;
using namespace esp_brookesia::systems::speaker;
using namespace esp_brookesia::ai_framework;

namespace
{

    constexpr const char *TAG = "VoiceMini";
    constexpr const char *PROMPT_URL = "file://spiffs/response_wo_zai_ting_ne.mp3";
    constexpr int SOUND_VOLUME_MIN = 0;
    constexpr int SOUND_VOLUME_MAX = 100;
    constexpr int SOUND_VOLUME_DEFAULT = 60;

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

    bool init_wifi()
    {
        ESP_LOGI(TAG, "Initialize WiFi");

        // Initialize TCP/IP stack
        ESP_ERROR_CHECK(esp_netif_init());

        // Create default event loop
        ESP_ERROR_CHECK(esp_event_loop_create_default());

        // Create default WiFi station interface
        esp_netif_create_default_wifi_sta();

        // Initialize WiFi
        wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
        ESP_ERROR_CHECK(esp_wifi_init(&cfg));

        // Set WiFi mode to station
        ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_STA));

        // Configure WiFi station
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
                .pmf_cfg = {.capable = true, .required = false}},
        };
        ESP_ERROR_CHECK(esp_wifi_set_config(WIFI_IF_STA, &wifi_config));

        // Start WiFi
        ESP_ERROR_CHECK(esp_wifi_start());

        // Connect to WiFi
        ESP_ERROR_CHECK(esp_wifi_connect());

        ESP_LOGI(TAG, "WiFi initialized, connecting to SSID: yql");

        // Wait for WiFi connection
        ESP_LOGI(TAG, "Waiting for WiFi connection...");
        vTaskDelay(pdMS_TO_TICKS(3000));

        // Initialize and start NTP client
        ESP_LOGI(TAG, "Initialize NTP client");

        // Configure timezone
        setenv("TZ", "CST-8", 1);
        tzset();

        // Initialize NTP using esp_netif_sntp API
        esp_sntp_config_t sntp_config = {
            .smooth_sync = false,
            .server_from_dhcp = false,
            .wait_for_sync = true,
            .start = true,
            .sync_cb = NULL,
            .renew_servers_after_new_IP = false,
            .ip_event_to_renew = IP_EVENT_STA_GOT_IP,
            .index_of_first_server = 0,
            .num_of_servers = 1,
            .servers = {"pool.ntp.org"}};

        ESP_ERROR_CHECK(esp_netif_sntp_init(&sntp_config));

        // Wait for time to be synchronized
        ESP_LOGI(TAG, "Waiting for time synchronization...");
        int retry = 0;
        const int retry_count = 60;

        while ((esp_netif_sntp_sync_wait(pdMS_TO_TICKS(1000)) != ESP_OK) && (++retry < retry_count))
        {
            ESP_LOGI(TAG, "Waiting for system time to be set... (%d/%d)", retry, retry_count);
        }

        if (retry < retry_count)
        {
            // Get and print current time
            time_t now;
            struct tm timeinfo;
            char strftime_buf[64];

            time(&now);
            localtime_r(&now, &timeinfo);
            strftime(strftime_buf, sizeof(strftime_buf), "%c", &timeinfo);
            ESP_LOGI(TAG, "Time synchronized: %s", strftime_buf);
        }
        else
        {
            ESP_LOGW(TAG, "Time synchronization failed after %d attempts", retry_count);
        }

        return true;
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

    bool load_coze_agent_config()
    {
        coze_agent_config_t config = {};
        CozeChatAgentInfo agent_info = {};
        std::vector<CozeChatRobotInfo> robot_infos;

        // ESP_UTILS_LOGW("Failed to read bot config from flash, use default config");
        agent_info.custom_consumer = COZE_AGENT_CUSTOM_CONSUMER;
        agent_info.app_id = COZE_AGENT_APP_ID;
        agent_info.public_key = COZE_AGENT_PUBLIC_KEY;
        agent_info.private_key = std::string(private_key_pem_start, private_key_pem_end - private_key_pem_start);

        robot_infos.push_back(CozeChatRobotInfo{
            .name = COZE_AGENT_BOT1_NAME,
            .bot_id = COZE_AGENT_BOT1_ID,
            .voice_id = COZE_AGENT_BOT1_VOICE_ID,
            .description = COZE_AGENT_BOT1_DESCRIPTION,
        });

        ESP_ERROR_CHECK(Agent::requestInstance()->configCozeAgentConfig(agent_info, robot_infos) ? ESP_OK : ESP_FAIL);

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
    ESP_LOGI(TAG, "Boot speaker voice mini demo");
    /* Initialize power for peripherals, including audio codec */
    ESP_ERROR_CHECK(bsp_power_init(true));

    auto &storage_service = StorageNVS::requestInstance();
    ESP_ERROR_CHECK(storage_service.begin() ? ESP_OK : ESP_FAIL);
    ESP_ERROR_CHECK(mount_spiffs() ? ESP_OK : ESP_FAIL);

    /* Initialize WiFi connection */
    ESP_ERROR_CHECK(init_wifi() ? ESP_OK : ESP_FAIL);

    ESP_ERROR_CHECK(init_audio_pipeline() ? ESP_OK : ESP_FAIL);

    /* Load Coze agent configuration */
    ESP_ERROR_CHECK(load_coze_agent_config() ? ESP_OK : ESP_FAIL);

    /* Initialize AI framework */
    auto ai_buddy = AI_Buddy::requestInstance();
    ESP_ERROR_CHECK(ai_buddy != nullptr ? ESP_OK : ESP_FAIL);

    /* Initialize AI Buddy with expression data */
    AI_Buddy::Data ai_data = {};
    ESP_ERROR_CHECK(ai_buddy->begin(ai_data) ? ESP_OK : ESP_FAIL);

    /* Start AI Buddy */
    ESP_ERROR_CHECK(ai_buddy->resume() ? ESP_OK : ESP_FAIL);

    /* Get agent instance and send Start event to connect to Coze platform */
    auto agent = Agent::requestInstance();
    ESP_ERROR_CHECK(agent != nullptr ? ESP_OK : ESP_FAIL);

    /* Send Init event */
    /* Note: Don't use ESP_ERROR_CHECK here as it may fail due to timeouts or system issues */
    bool init_result = agent->sendChatEvent(Agent::ChatEvent::Init, true, 1000);
    if (!init_result)
    {
        ESP_LOGW(TAG, "Failed to init Coze chat, will continue execution");
    }
    else
    {
        ESP_LOGI(TAG, "Coze chat init successfully");
    }

    /* Send Start event to connect to Coze platform */
    /* Note: Don't use ESP_ERROR_CHECK here as it may fail due to invalid key or network issues */
    bool start_result = agent->sendChatEvent(Agent::ChatEvent::Start, true, 5000);
    if (!start_result)
    {
        ESP_LOGW(TAG, "Failed to start Coze chat, will retry in background");
    }
    else
    {
        ESP_LOGI(TAG, "Coze chat started successfully");
    }

    while (true)
    {
        vTaskDelay(pdMS_TO_TICKS(1000));
    }
}
