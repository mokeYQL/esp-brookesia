/*
 * 知识点：constexpr编译期常量
 * 源项目：e:\3.esp-brookesia\esp-brookesia\products\speaker\main\modules\system.cpp 第31-36行
 * C++ Primer中文版（第五版）：第2章 变量和基本类型 - 2.4 const限定符
 * Effective+C++中文版（第三版）：条款2 尽量以const、enum、inline替换#define
 */
#include <iostream>
#include <string_view>

int main() {
    // constexpr字符串指针
    constexpr const char* FUNCTION_OPEN_APP_THREAD_NAME = "open_app";
    // constexpr整数
    constexpr int FUNCTION_OPEN_APP_THREAD_STACK_SIZE = 20 * 1024;
    constexpr int FUNCTION_OPEN_APP_WAIT_SPEAKING_PRE_MS = 2000;
    constexpr int FUNCTION_OPEN_APP_WAIT_SPEAKING_INTERVAL_MS = 10;
    constexpr int FUNCTION_OPEN_APP_WAIT_SPEAKING_MAX_MS = 2000;
    constexpr bool FUNCTION_OPEN_APP_THREAD_STACK_CAPS_EXT = true;
    
    // 计算最大等待次数（在编译期）
    constexpr int MAX_WAIT_COUNT = FUNCTION_OPEN_APP_WAIT_SPEAKING_MAX_MS / FUNCTION_OPEN_APP_WAIT_SPEAKING_INTERVAL_MS;
    
    std::cout << "Thread Name: " << FUNCTION_OPEN_APP_THREAD_NAME << std::endl;
    std::cout << "Stack Size: " << FUNCTION_OPEN_APP_THREAD_STACK_SIZE << " bytes" << std::endl;
    std::cout << "Wait Pre MS: " << FUNCTION_OPEN_APP_WAIT_SPEAKING_PRE_MS << " ms" << std::endl;
    std::cout << "Wait Interval: " << FUNCTION_OPEN_APP_WAIT_SPEAKING_INTERVAL_MS << " ms" << std::endl;
    std::cout << "Wait Max: " << FUNCTION_OPEN_APP_WAIT_SPEAKING_MAX_MS << " ms" << std::endl;
    std::cout << "Stack Caps Ext: " << (FUNCTION_OPEN_APP_THREAD_STACK_CAPS_EXT ? "true" : "false") << std::endl;
    std::cout << "Max Wait Count: " << MAX_WAIT_COUNT << std::endl;
    
    // 演示constexpr字符串视图
    constexpr std::string_view THREAD_NAME_VIEW = FUNCTION_OPEN_APP_THREAD_NAME;
    std::cout << "Thread Name View Length: " << THREAD_NAME_VIEW.length() << std::endl;
    
    return 0;
}
