/*
 * 知识点：条件编译
 * 源项目：e:\3.esp-brookesia\esp-brookesia\core\brookesia_core\esp_brookesia.hpp 第23-25行
 * C++ Primer中文版（第五版）：第16章 模板与泛型编程 - 16.1 定义模板
 * Effective+C++中文版（第三版）：无直接对应章节
 */

#include <iostream>
#include <string>

// 1. 基本条件编译示例
#define ENABLE_DEBUG 1 // 定义宏
#define SCREEN_WIDTH 320
#define SCREEN_HEIGHT 240

// 2. 特性开关
#define ESP_BROOKESIA_SERVICES_ENABLE_STORAGE_NVS 1
#define ESP_BROOKESIA_SYSTEMS_ENABLE_PHONE 0
#define ESP_BROOKESIA_SYSTEMS_ENABLE_SPEAKER 1

// 3. 版本检查
#define APP_VERSION_MAJOR 1
#define APP_VERSION_MINOR 0
#define APP_VERSION_PATCH 0

class App
{
public:
    App(const std::string &name) : _name(name)
    {
        std::cout << "App " << _name << " created" << std::endl;

        // 4. 调试信息：仅在调试模式下输出
#ifdef ENABLE_DEBUG
        std::cout << "[DEBUG] App constructor called" << std::endl;
        std::cout << "[DEBUG] Screen size: " << SCREEN_WIDTH << "x" << SCREEN_HEIGHT << std::endl;
#endif
    }

    ~App()
    {
        std::cout << "App " << _name << " destroyed" << std::endl;

#ifdef ENABLE_DEBUG
        std::cout << "[DEBUG] App destructor called" << std::endl;
#endif
    }

    void run()
    {
        std::cout << "App " << _name << " running" << std::endl;

        // 5. 根据宏定义选择不同的代码路径
#if APP_VERSION_MAJOR > 0
        std::cout << "Running on version " << APP_VERSION_MAJOR << "." << APP_VERSION_MINOR << "." << APP_VERSION_PATCH << std::endl;
#else
        std::cout << "Running on legacy version" << std::endl;
#endif
    }

    // 6. 根据不同系统启用不同功能
    void initializeServices()
    {
        std::cout << "Initializing services..." << std::endl;

#if ESP_BROOKESIA_SERVICES_ENABLE_STORAGE_NVS
        std::cout << "  - Storage NVS service enabled" << std::endl;
        // 在这里添加NVS服务初始化代码
#else
        std::cout << "  - Storage NVS service disabled" << std::endl;
#endif

#if ESP_BROOKESIA_SYSTEMS_ENABLE_PHONE
        std::cout << "  - Phone system enabled" << std::endl;
        // 在这里添加电话系统初始化代码
#else
        std::cout << "  - Phone system disabled" << std::endl;
#endif

#if ESP_BROOKESIA_SYSTEMS_ENABLE_SPEAKER
        std::cout << "  - Speaker system enabled" << std::endl;
        // 在这里添加扬声器系统初始化代码
#else
        std::cout << "  - Speaker system disabled" << std::endl;
#endif
    }

    // 7. 条件函数定义
#ifdef ENABLE_DEBUG
    void debugInfo()
    {
        std::cout << "[DEBUG] App debug info:" << std::endl;
        std::cout << "[DEBUG]   Name: " << _name << std::endl;
        std::cout << "[DEBUG]   ID: " << _id << std::endl;
    }
#endif

    std::string getName() const
    {
        return _name;
    }

private:
    int _id = 0;
    std::string _name;
};

// 8. 条件包含头文件
#ifdef ESP_BROOKESIA_SERVICES_ENABLE_STORAGE_NVS
// #include "services/storage_nvs/esp_brookesia_service_storage_nvs.hpp"
#endif

// 9. 宏的条件定义
#ifndef MAX_APPS
#define MAX_APPS 10
#endif

// 10. 多层条件编译
#if defined(ESP_BROOKESIA_SYSTEMS_ENABLE_PHONE) && ESP_BROOKESIA_SYSTEMS_ENABLE_PHONE
#define SYSTEM_TYPE "Phone"
#elif defined(ESP_BROOKESIA_SYSTEMS_ENABLE_SPEAKER) && ESP_BROOKESIA_SYSTEMS_ENABLE_SPEAKER
#define SYSTEM_TYPE "Speaker"
#else
#define SYSTEM_TYPE "Base"
#endif

// 11. 编译时断言
#if SCREEN_WIDTH < 100 || SCREEN_HEIGHT < 100
#error "Screen size is too small"
#endif

// 12. 宏函数的条件定义
#ifdef ENABLE_DEBUG
#define LOG_DEBUG(message) std::cout << "[DEBUG] " << message << std::endl
#else
#define LOG_DEBUG(message) // 空宏，调试模式下不执行
#endif

#define LOG_INFO(message) std::cout << "[INFO] " << message << std::endl
#define LOG_ERROR(message) std::cout << "[ERROR] " << message << std::endl

int main()
{
    std::cout << "=== Conditional Compilation Examples ===" << std::endl;

    // 13. 使用宏定义的值
    std::cout << "\n1. System Type: " << SYSTEM_TYPE << std::endl;
    std::cout << "2. Max Apps: " << MAX_APPS << std::endl;

    // 14. 使用条件编译的日志宏
    LOG_INFO("Starting application...");
    LOG_DEBUG("Debug logging enabled");

    // 创建应用实例
    App app("TestApp");

    // 15. 调用条件编译的函数
#ifdef ENABLE_DEBUG
    app.debugInfo();
#endif

    // 运行应用
    app.run();
    app.initializeServices();

    // 16. 条件编译的代码块
#ifdef ENABLE_DEBUG
    std::cout << "\n=== Debug Information ===" << std::endl;
    std::cout << "Compiler: " << __VERSION__ << std::endl;
    // 使用预处理指令确定平台
#if defined(_WIN32)
    std::cout << "Platform: Windows" << std::endl;
#elif defined(__linux__)
    std::cout << "Platform: Linux" << std::endl;
#else
    std::cout << "Platform: Unknown" << std::endl;
#endif
    std::cout << "Screen Size: " << SCREEN_WIDTH << "x" << SCREEN_HEIGHT << std::endl;
#endif

    LOG_INFO("Application finished");

    return 0;
}