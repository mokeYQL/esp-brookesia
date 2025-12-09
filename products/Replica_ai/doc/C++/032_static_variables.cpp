/*
 * 知识点：静态变量与RTC_NOINIT_ATTR
 * 源项目：e:\3.esp-brookesia\esp-brookesia\products\speaker\main\modules\system.cpp 第61行
 * C++ Primer中文版（第五版）：第2章 变量和基本类型 - 2.3 变量声明和定义的关系
 * Effective+C++中文版（第三版）：条款4 确定对象被使用前已先被初始化
 */
#include <iostream>

// 模拟ESP32的RTC_NOINIT_ATTR宏，用于标记变量在重启后保持值
#define RTC_NOINIT_ATTR __attribute__((section(".rtc_noinit")))

// 模拟开发者模式密钥
constexpr int DEVELOPER_MODE_KEY = 0x655;

// 这个变量在软件重启时不会被初始化
static RTC_NOINIT_ATTR int developer_mode_key = 0;

// 检查是否进入开发者模式
static bool check_whether_enter_developer_mode() {
    if (developer_mode_key != DEVELOPER_MODE_KEY) {
        std::cout << "Developer mode disabled" << std::endl;
        return true;
    }
    
    std::cout << "Developer mode enabled" << std::endl;
    // 在这里可以添加开发者模式的逻辑
    return true;
}

// 进入开发者模式的函数
static void enter_developer_mode() {
    std::cout << "Entering developer mode..." << std::endl;
    developer_mode_key = DEVELOPER_MODE_KEY;
    std::cout << "Rebooting..." << std::endl;
    // 模拟重启后的值保持
    std::cout << "After reboot, developer_mode_key = " << developer_mode_key << std::endl;
}

int main() {
    std::cout << "Initial developer_mode_key = " << developer_mode_key << std::endl;
    
    // 首次检查
    check_whether_enter_developer_mode();
    
    // 进入开发者模式
    enter_developer_mode();
    
    // 再次检查
    check_whether_enter_developer_mode();
    
    return 0;
}
