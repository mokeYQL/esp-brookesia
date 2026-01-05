/*
 * 知识点：条件编译
 * 源项目：e:\3.esp-brookesia\esp-brookesia\products\speaker\main\modules\system.cpp 第62-65行
 * C++ Primer中文版（第五版）：第16章 模板与泛型编程 - 16.1 定义模板
 * Effective+C++中文版（第三版）：无直接对应章节
 */
#include <iostream>
#include <string>
#include <vector>

// 模拟不同的配置选项
#define COZE_AGENT_ENABLE_DEFAULT_CONFIG 1
#define COZE_AGENT_CUSTOM_CONSUMER "test_consumer"
#define COZE_AGENT_APP_ID "test_app_id"
#define COZE_AGENT_DEVICE_PUBLIC_KEY "test_public_key"
#define COZE_AGENT_BOT1_ENABLE 1
#define COZE_AGENT_BOT1_NAME "Bot1"
#define COZE_AGENT_BOT1_ID "bot1_id"
#define COZE_AGENT_BOT1_VOICE_ID "voice1"
#define COZE_AGENT_BOT1_DESCRIPTION "Test Bot 1"
#define COZE_AGENT_BOT2_ENABLE 0

// 模拟外部符号
const char private_key_pem_start[] = "-----BEGIN PRIVATE KEY-----\nTestKey\n-----END PRIVATE KEY-----\n";
const char private_key_pem_end[] = ""; // 模拟结束位置

struct CozeChatRobotInfo
{
    std::string name;
    std::string bot_id;
    std::string voice_id;
    std::string description;
};

struct CozeChatAgentInfo
{
    std::string custom_consumer;
    std::string app_id;
    std::string public_key;
    std::string private_key;
};

bool load_coze_agent_config()
{
    CozeChatAgentInfo agent_info = {};
    std::vector<CozeChatRobotInfo> robot_infos;

    bool config_loaded = false;

    // 模拟从flash读取配置失败
    if (!config_loaded)
    {
        std::cout << "Failed to read bot config from flash" << std::endl;

#if COZE_AGENT_ENABLE_DEFAULT_CONFIG
        std::cout << "Using default config" << std::endl;

        // 使用默认配置
        agent_info.custom_consumer = COZE_AGENT_CUSTOM_CONSUMER;
        agent_info.app_id = COZE_AGENT_APP_ID;
        agent_info.public_key = COZE_AGENT_DEVICE_PUBLIC_KEY;

        // 从外部符号加载私钥
        agent_info.private_key = std::string(private_key_pem_start, private_key_pem_end - private_key_pem_start);

#if COZE_AGENT_BOT1_ENABLE
        // 添加Bot1
        robot_infos.push_back(CozeChatRobotInfo{
            .name = COZE_AGENT_BOT1_NAME,
            .bot_id = COZE_AGENT_BOT1_ID,
            .voice_id = COZE_AGENT_BOT1_VOICE_ID,
            .description = COZE_AGENT_BOT1_DESCRIPTION,
        });
        std::cout << "Added Bot1: " << COZE_AGENT_BOT1_NAME << std::endl;
#endif // COZE_AGENT_BOT1_ENABLE

#if COZE_AGENT_BOT2_ENABLE
        // 添加Bot2（这个条件不满足，不会被编译）
        robot_infos.push_back(CozeChatRobotInfo{
            .name = COZE_AGENT_BOT2_NAME,
            .bot_id = COZE_AGENT_BOT2_ID,
            .voice_id = COZE_AGENT_BOT2_VOICE_ID,
            .description = COZE_AGENT_BOT2_DESCRIPTION,
        });
        std::cout << "Added Bot2: " << COZE_AGENT_BOT2_NAME << std::endl;
#endif // COZE_AGENT_BOT2_ENABLE

#else  // COZE_AGENT_ENABLE_DEFAULT_CONFIG
        std::cerr << "No default config available" << std::endl;
        return false;
#endif // COZE_AGENT_ENABLE_DEFAULT_CONFIG
    }

    // 打印加载的配置
    std::cout << "\nLoaded config:" << std::endl;
    std::cout << "Consumer: " << agent_info.custom_consumer << std::endl;
    std::cout << "App ID: " << agent_info.app_id << std::endl;
    std::cout << "Public Key: " << agent_info.public_key << std::endl;
    std::cout << "Private Key: [hidden]" << std::endl;
    std::cout << "Bots loaded: " << robot_infos.size() << std::endl;

    for (const auto &bot : robot_infos)
    {
        std::cout << "  - Bot: " << bot.name << " (ID: " << bot.bot_id << ")" << std::endl;
    }

    return true;
}

int main()
{
    std::cout << "=== Testing Conditional Compilation ===" << std::endl;

    if (load_coze_agent_config())
    {
        std::cout << "\nConfig loaded successfully" << std::endl;
    }
    else
    {
        std::cout << "\nFailed to load config" << std::endl;
    }

    return 0;
}
