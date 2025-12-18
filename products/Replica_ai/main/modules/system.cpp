/*
 * SPDX-FileCopyrightText: 2025 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: CC0-1.0
 */
#include <string>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "soc/soc_caps.h"
#if SOC_USB_SERIAL_JTAG_SUPPORTED
#include "soc/usb_serial_jtag_reg.h"
#include "hal/usb_serial_jtag_ll.h"
#endif
#include "esp_private/usb_phy.h"
#include "bsp/esp-bsp.h"
#include "esp_brookesia.hpp"
#include "esp_brookesia_app_settings.hpp"
#ifdef ESP_UTILS_LOG_TAG
#undef ESP_UTILS_LOG_TAG
#endif
#define ESP_UTILS_LOG_TAG "System"
#include "esp_lib_utils.h"
#include "coze_agent_config.h"
#include "coze_agent_config_default.h"
// #include "usb_msc.h"
#include "system.hpp"

#include "battery_monitor.h"
// #include "imu_gesture.h"
// #include "touch_sensor.h"
// #include "led_indicator.h"

constexpr const char *FUNCTION_OPEN_APP_THREAD_NAME = "open_app";
constexpr int FUNCTION_OPEN_APP_THREAD_STACK_SIZE = 20 * 1024;
constexpr int FUNCTION_OPEN_APP_WAIT_SPEAKING_PRE_MS = 2000;
constexpr int FUNCTION_OPEN_APP_WAIT_SPEAKING_INTERVAL_MS = 10;
constexpr int FUNCTION_OPEN_APP_WAIT_SPEAKING_MAX_MS = 2000;
constexpr bool FUNCTION_OPEN_APP_THREAD_STACK_CAPS_EXT = true;

constexpr const char *FUNCTION_VOLUME_CHANGE_THREAD_NAME = "volume_change";
constexpr size_t FUNCTION_VOLUME_CHANGE_THREAD_STACK_SIZE = 6 * 1024;
constexpr bool FUNCTION_VOLUME_CHANGE_THREAD_STACK_CAPS_EXT = true;
constexpr int FUNCTION_VOLUME_CHANGE_STEP = 20;

constexpr const char *FUNCTION_BRIGHTNESS_CHANGE_THREAD_NAME = "brightness_change";
constexpr size_t FUNCTION_BRIGHTNESS_CHANGE_THREAD_STACK_SIZE = 6 * 1024;
constexpr bool FUNCTION_BRIGHTNESS_CHANGE_THREAD_STACK_CAPS_EXT = true;
constexpr int FUNCTION_BRIGHTNESS_CHANGE_STEP = 30;

constexpr int DEVELOPER_MODE_KEY = 0x655;

using namespace esp_brookesia;
using namespace esp_brookesia::systems::speaker;
using namespace esp_brookesia::gui;
using namespace esp_brookesia::apps;
using namespace esp_brookesia::services;
using namespace esp_brookesia::ai_framework;

static RTC_NOINIT_ATTR int developer_mode_key;
#if COZE_AGENT_ENABLE_DEFAULT_CONFIG
extern const char private_key_pem_start[] asm("_binary_private_key_pem_start");
extern const char private_key_pem_end[] asm("_binary_private_key_pem_end");
#endif

static bool load_coze_agent_config();

/**
 * This variable is used to store a special key which indicates whether to enter developer mode.
 * When the device is rebooted by software, this variable will not be initialized.
 */

static BatteryMonitor battery_monitor;

bool system_init()
{
    ESP_UTILS_LOG_TRACE_GUARD();

    /* Create a speaker object */
    Speaker *speaker = nullptr;
    ESP_UTILS_CHECK_EXCEPTION_RETURN(
        speaker = new Speaker(), false, "Create speaker failed");

    // battery_monitor.setBatteryShutdownCallback([speaker]()
    //                                            { show_low_power(speaker); });
    ESP_UTILS_CHECK_FALSE_RETURN(battery_monitor.init(), false, "Battery monitor init failed");
    // ESP_UTILS_CHECK_FALSE_RETURN(imu_gesture.init(), false, "IMU gesture init failed");
    // ESP_UTILS_CHECK_FALSE_RETURN(touch_sensor.init(), false, "Touch sensor init failed");
    // ESP_UTILS_CHECK_FALSE_RETURN(check_whether_enter_developer_mode(), false, "Check whether enter developer mode failed");

    /* Load coze agent config */
    if (!load_coze_agent_config())
    {
        ESP_UTILS_LOGE("Load coze agent config failed");
    }

    std::unique_ptr<Stylesheet> stylesheet;
    ESP_UTILS_CHECK_EXCEPTION_RETURN(
        stylesheet = std::make_unique<Stylesheet>(ESP_BROOKESIA_SPEAKER_360_360_DARK_STYLESHEET), false,
        "Create stylesheet failed");
    ESP_UTILS_LOGI("Using stylesheet (%s)", stylesheet->core.name);
    ESP_UTILS_CHECK_FALSE_RETURN(speaker->addStylesheet(stylesheet.get()), false, "Add stylesheet failed");
    ESP_UTILS_CHECK_FALSE_RETURN(speaker->activateStylesheet(stylesheet.get()), false, "Activate stylesheet failed");

    stylesheet = nullptr;

    /* Begin the speaker */
    LvLockGuard gui_guard;
    ESP_UTILS_CHECK_FALSE_RETURN(speaker->begin(), false, "Begin failed");

    return true;
}

static bool load_coze_agent_config()
{
    ESP_UTILS_LOG_TRACE_GUARD();

    coze_agent_config_t config = {};
    CozeChatAgentInfo agent_info = {};
    std::vector<CozeChatRobotInfo> robot_infos;

    if (coze_agent_config_read(&config) == ESP_OK)
    {
        agent_info.custom_consumer = config.custom_consumer ? config.custom_consumer : "";
        agent_info.app_id = config.appid ? config.appid : "";
        agent_info.public_key = config.public_key ? config.public_key : "";
        agent_info.private_key = config.private_key ? config.private_key : "";
        for (int i = 0; i < config.bot_num; i++)
        {
            robot_infos.push_back(CozeChatRobotInfo{
                .name = config.bot[i].bot_name ? config.bot[i].bot_name : "",
                .bot_id = config.bot[i].bot_id ? config.bot[i].bot_id : "",
                .voice_id = config.bot[i].voice_id ? config.bot[i].voice_id : "",
                .description = config.bot[i].bot_description ? config.bot[i].bot_description : "",
            });
        }
        ESP_UTILS_CHECK_FALSE_RETURN(coze_agent_config_release(&config) == ESP_OK, false, "Release bot config failed");
    }
    else
    {
#if COZE_AGENT_ENABLE_DEFAULT_CONFIG
        ESP_UTILS_LOGW("Failed to read bot config from flash, use default config");
        agent_info.custom_consumer = COZE_AGENT_CUSTOM_CONSUMER;
        agent_info.app_id = COZE_AGENT_APP_ID;
        agent_info.public_key = COZE_AGENT_DEVICE_PUBLIC_KEY;
        agent_info.private_key = std::string(private_key_pem_start, private_key_pem_end - private_key_pem_start);
#if COZE_AGENT_BOT1_ENABLE
        robot_infos.push_back(CozeChatRobotInfo{
            .name = COZE_AGENT_BOT1_NAME,
            .bot_id = COZE_AGENT_BOT1_ID,
            .voice_id = COZE_AGENT_BOT1_VOICE_ID,
            .description = COZE_AGENT_BOT1_DESCRIPTION,
        });
#endif // COZE_AGENT_BOT1_ENABLE
#if COZE_AGENT_BOT2_ENABLE
        robot_infos.push_back(CozeChatRobotInfo{
            .name = COZE_AGENT_BOT2_NAME,
            .bot_id = COZE_AGENT_BOT2_ID,
            .voice_id = COZE_AGENT_BOT2_VOICE_ID,
            .description = COZE_AGENT_BOT2_DESCRIPTION,
        });
#endif // COZE_AGENT_BOT2_ENABLE
#else
        ESP_UTILS_CHECK_FALSE_RETURN(false, false, "Failed to read bot config");
#endif // COZE_AGENT_ENABLE_DEFAULT_CONFIG
    }

    ESP_UTILS_CHECK_FALSE_RETURN(
        Agent::requestInstance()->configCozeAgentConfig(agent_info, robot_infos), false, "Config coze agent failed");

    return true;
}

bool system_check_is_developer_mode()
{
    return (developer_mode_key == DEVELOPER_MODE_KEY);
}

static void _usb_serial_jtag_phy_init()
{
    SET_PERI_REG_MASK(USB_SERIAL_JTAG_CONF0_REG, USB_SERIAL_JTAG_PAD_PULL_OVERRIDE);
    CLEAR_PERI_REG_MASK(USB_SERIAL_JTAG_CONF0_REG, USB_SERIAL_JTAG_DP_PULLUP);
    SET_PERI_REG_MASK(USB_SERIAL_JTAG_CONF0_REG, USB_SERIAL_JTAG_DP_PULLDOWN);
    vTaskDelay(pdMS_TO_TICKS(10));
#if USB_SERIAL_JTAG_LL_EXT_PHY_SUPPORTED
    usb_serial_jtag_ll_phy_enable_external(false); // Use internal PHY
    usb_serial_jtag_ll_phy_enable_pad(true);       // Enable USB PHY pads
#else                                              // USB_SERIAL_JTAG_LL_EXT_PHY_SUPPORTED
    usb_serial_jtag_ll_phy_set_defaults(); // External PHY not supported. Set default values.
#endif                                             // USB_WRAP_LL_EXT_PHY_SUPPORTED
    CLEAR_PERI_REG_MASK(USB_SERIAL_JTAG_CONF0_REG, USB_SERIAL_JTAG_DP_PULLDOWN);
    SET_PERI_REG_MASK(USB_SERIAL_JTAG_CONF0_REG, USB_SERIAL_JTAG_DP_PULLUP);
    CLEAR_PERI_REG_MASK(USB_SERIAL_JTAG_CONF0_REG, USB_SERIAL_JTAG_PAD_PULL_OVERRIDE);
}

void restart_usb_serial_jtag()
{
    gpio_config_t io_usb_power_conf = {
        .pin_bit_mask = (1ULL << BSP_USB_DP),
        .mode = GPIO_MODE_OUTPUT,
        .pull_up_en = GPIO_PULLUP_DISABLE,
        .pull_down_en = GPIO_PULLDOWN_DISABLE,
        .intr_type = GPIO_INTR_DISABLE};
    gpio_config(&io_usb_power_conf);
    gpio_set_level(BSP_USB_DP, 0);
    _usb_serial_jtag_phy_init();
}
