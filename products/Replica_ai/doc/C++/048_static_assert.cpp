/*
 * 知识点：static_assert编译期断言
 * 源项目：e:\3.esp-brookesia\esp-brookesia\products\speaker\main\modules\system.cpp 第31-36行
 * C++ Primer中文版（第五版）：第2章 变量和基本类型 - 2.4 const限定符
 * Effective+C++中文版（第三版）：条款02 尽量以const、enum、inline替换#define
 */
#include <iostream>
#include <type_traits>

// 模拟硬件相关的常量
constexpr size_t MAX_THREAD_STACK_SIZE = 32 * 1024;
constexpr size_t MIN_THREAD_STACK_SIZE = 4 * 1024;
constexpr bool HARDWARE_SUPPORTS_EXTENDED_STACK = true;

// 线程配置结构体
struct ThreadConfig
{
    const char *name;
    size_t stack_size;
    bool stack_in_ext;
};

// 编译期检查线程栈大小
constexpr bool is_valid_stack_size(size_t stack_size)
{
    return stack_size >= MIN_THREAD_STACK_SIZE && stack_size <= MAX_THREAD_STACK_SIZE;
}

// 演示static_assert的使用
void demonstrate_static_assert()
{
    std::cout << "=== Testing static_assert ===" << std::endl;

    // 1. 检查常量表达式
    static_assert(MAX_THREAD_STACK_SIZE > MIN_THREAD_STACK_SIZE,
                  "MAX_THREAD_STACK_SIZE must be greater than MIN_THREAD_STACK_SIZE");

    // 2. 检查类型特性
    static_assert(std::is_same_v<int, int>, "int should be the same as int");
    static_assert(!std::is_same_v<int, double>, "int should not be the same as double");
    static_assert(std::is_class_v<ThreadConfig>, "ThreadConfig should be a class");
    static_assert(std::is_pointer_v<const char *>, "const char* should be a pointer");

    // 3. 检查硬件特性
    static_assert(HARDWARE_SUPPORTS_EXTENDED_STACK,
                  "Hardware must support extended stack");

    // 4. 检查常量值范围
    static_assert(MIN_THREAD_STACK_SIZE <= 4096,
                  "MIN_THREAD_STACK_SIZE must be <= 4096");

    std::cout << "All static_assert checks passed!" << std::endl;
}

// 编译期配置线程
constexpr ThreadConfig configure_thread(const char *name, size_t stack_size, bool stack_in_ext)
{
    // 编译期条件
    return {
        .name = name,
        .stack_size = stack_size,
        .stack_in_ext = stack_in_ext && HARDWARE_SUPPORTS_EXTENDED_STACK};
}

// 测试不同的线程配置
void test_thread_configurations()
{
    std::cout << "\n=== Testing Thread Configurations ===" << std::endl;

    // 有效配置
    constexpr ThreadConfig app_thread = configure_thread(
        "open_app",
        20 * 1024, // 20KB
        true);

    std::cout << "App Thread:" << std::endl;
    std::cout << "  Name: " << app_thread.name << std::endl;
    std::cout << "  Stack Size: " << app_thread.stack_size << " bytes" << std::endl;
    std::cout << "  Stack in Ext: " << (app_thread.stack_in_ext ? "true" : "false") << std::endl;

    // 另一个有效配置
    constexpr ThreadConfig volume_thread = configure_thread(
        "volume_change",
        6 * 1024, // 6KB
        true);

    std::cout << "\nVolume Thread:" << std::endl;
    std::cout << "  Name: " << volume_thread.name << std::endl;
    std::cout << "  Stack Size: " << volume_thread.stack_size << " bytes" << std::endl;
    std::cout << "  Stack in Ext: " << (volume_thread.stack_in_ext ? "true" : "false") << std::endl;
}

int main()
{
    demonstrate_static_assert();
    test_thread_configurations();

    return 0;
}
