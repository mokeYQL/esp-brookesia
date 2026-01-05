/*
 * 知识点：consteval和constinit编译期特性
 * 源项目：e:\3.esp-brookesia\esp-brookesia\products\speaker\main\modules\system.cpp 第31-36行
 * C++ Primer中文版（第五版）：第2章 变量和基本类型 - 2.4 const限定符
 * Effective+C++中文版（第三版）：条款02 尽量以const、enum、inline替换#define
 */
#include <iostream>
#include <string_view>

// 1. 使用constexpr创建编译期函数
constexpr int calculate_max_wait_count(int max_ms, int interval_ms)
{
    return max_ms / interval_ms;
}

// 2. 使用constexpr初始化全局变量
constexpr const char *GLOBAL_THREAD_NAME = "system_thread";
constexpr int GLOBAL_DEFAULT_VOLUME = 50;

// 3. 模拟硬件相关的常量计算
constexpr int HARDWARE_TIMER_FREQUENCY = 1000; // 1kHz
constexpr int MAX_DELAY_MS = 5000;

// constexpr函数确保在编译期计算
constexpr int get_max_timer_ticks()
{
    return MAX_DELAY_MS * HARDWARE_TIMER_FREQUENCY / 1000;
}

// 线程配置结构体
struct ThreadConfig
{
    const char *name;
    size_t stack_size;
    bool stack_in_ext;
};

// 编译期配置线程
constexpr ThreadConfig create_thread_config(const char *name, size_t stack_size, bool stack_in_ext)
{
    return {
        name,
        stack_size,
        stack_in_ext};
}

// 演示consteval和constinit的使用
void demonstrate_const_features()
{
    std::cout << "=== Testing consteval and constinit ===" << std::endl;

    // 使用consteval函数在编译期计算
    constexpr int max_wait_count = calculate_max_wait_count(2000, 10);
    std::cout << "Max wait count (consteval): " << max_wait_count << std::endl;

    // 使用constinit变量
    std::cout << "Global thread name (constinit): " << GLOBAL_THREAD_NAME << std::endl;
    std::cout << "Global default volume (constinit): " << GLOBAL_DEFAULT_VOLUME << std::endl;

    // 演示consteval函数用于硬件配置
    constexpr int max_timer_ticks = get_max_timer_ticks();
    std::cout << "Max timer ticks (consteval): " << max_timer_ticks << std::endl;

    // 演示编译期线程配置
    constexpr ThreadConfig app_thread_config = create_thread_config(
        "open_app",
        20 * 1024,
        true);

    std::cout << "\nCompiled thread config:" << std::endl;
    std::cout << "  Name: " << app_thread_config.name << std::endl;
    std::cout << "  Stack size: " << app_thread_config.stack_size << " bytes" << std::endl;
    std::cout << "  Stack in ext: " << (app_thread_config.stack_in_ext ? "true" : "false") << std::endl;
}

// 演示consteval函数的约束
void demonstrate_consteval_constraints()
{
    std::cout << "\n=== Testing consteval constraints ===" << std::endl;

    // 有效：编译期常量
    constexpr int result1 = calculate_max_wait_count(1000, 20);
    std::cout << "Valid consteval usage: " << result1 << std::endl;

    // 有效：constexpr变量
    constexpr int max_ms = 1500;
    constexpr int interval_ms = 25;
    constexpr int result2 = calculate_max_wait_count(max_ms, interval_ms);
    std::cout << "Valid consteval with constexpr variables: " << result2 << std::endl;

    // 注意：以下代码会编译失败，因为i是运行时变量
    /*
    int i = 100;
    int result3 = calculate_max_wait_count(i, 10); // 编译错误：consteval函数参数必须是编译期常量
    */
}

int main()
{
    demonstrate_const_features();
    demonstrate_consteval_constraints();

    return 0;
}
