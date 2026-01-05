/*
 * 知识点：boost::this_thread::sleep_for时间处理
 * 源项目：e:\3.esp-brookesia\esp-brookesia\products\speaker\main\modules\system.cpp 第246行
 * C++ Primer中文版（第五版）：第12章 动态内存 - 12.2 动态数组
 * Effective+C++中文版（第三版）：条款54 让自己熟悉TR1和Boost
 */
#include <iostream>
#include <chrono>
#include <thread>

// 模拟boost命名空间和相关功能
namespace boost
{
    namespace chrono
    {
        // 简化为固定的milliseconds类型
        using milliseconds = std::chrono::milliseconds;
    }

    namespace this_thread
    {
        void sleep_for(const std::chrono::milliseconds &d)
        {
            std::cout << "boost::this_thread::sleep_for called" << std::endl;
            // 实际使用std::this_thread::sleep_for实现
            std::this_thread::sleep_for(d);
        }
    }
}

// 模拟AI_Buddy类
class AI_Buddy
{
public:
    static AI_Buddy *requestInstance()
    {
        static AI_Buddy instance;
        return &instance;
    }

    bool isSpeaking()
    {
        // 模拟正在说话
        static int count = 0;
        return ++count < 3; // 前3次调用返回true
    }

private:
    AI_Buddy() = default;
};

// 演示boost::this_thread::sleep_for的使用
void demonstrate_boost_sleep()
{
    std::cout << "=== Testing boost::this_thread::sleep_for ===" << std::endl;

    // 模拟常量定义
    constexpr int FUNCTION_OPEN_APP_WAIT_SPEAKING_PRE_MS = 200;
    constexpr int FUNCTION_OPEN_APP_WAIT_SPEAKING_INTERVAL_MS = 50;
    constexpr int FUNCTION_OPEN_APP_WAIT_SPEAKING_MAX_MS = 500;

    std::cout << "Waiting for speaking to finish..." << std::endl;

    // 等待前延时
    std::cout << "Pre-wait: " << FUNCTION_OPEN_APP_WAIT_SPEAKING_PRE_MS << " ms" << std::endl;
    boost::this_thread::sleep_for(std::chrono::milliseconds(FUNCTION_OPEN_APP_WAIT_SPEAKING_PRE_MS));

    int wait_count = 0;
    int wait_interval_ms = FUNCTION_OPEN_APP_WAIT_SPEAKING_INTERVAL_MS;
    int wait_max_count = FUNCTION_OPEN_APP_WAIT_SPEAKING_MAX_MS / wait_interval_ms;

    AI_Buddy *ai_buddy = AI_Buddy::requestInstance();

    // 等待AI停止说话
    while ((wait_count < wait_max_count) && ai_buddy->isSpeaking())
    {
        std::cout << "Waiting iteration " << wait_count + 1 << "/" << wait_max_count << std::endl;
        boost::this_thread::sleep_for(std::chrono::milliseconds(wait_interval_ms));
        wait_count++;
    }

    if (ai_buddy->isSpeaking())
    {
        std::cout << "Timeout: AI is still speaking after max wait time" << std::endl;
    }
    else
    {
        std::cout << "AI stopped speaking after " << wait_count * wait_interval_ms << " ms" << std::endl;
    }
}

// 演示标准库的sleep_for
void demonstrate_std_sleep()
{
    std::cout << "\n=== Testing std::this_thread::sleep_for ===" << std::endl;

    auto start = std::chrono::high_resolution_clock::now();

    // 休眠100ms
    std::this_thread::sleep_for(std::chrono::milliseconds(100));

    auto end = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);

    std::cout << "Slept for approximately " << duration.count() << " ms" << std::endl;
}

int main()
{
    demonstrate_boost_sleep();
    demonstrate_std_sleep();

    return 0;
}
