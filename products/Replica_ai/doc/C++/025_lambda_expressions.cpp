/*
 * 知识点：Lambda表达式
 * 源项目：e:\3.esp-brookesia\esp-brookesia\products\speaker\main\modules\system.cpp 第88-89行
 * C++ Primer中文版（第五版）：第10章 泛型算法 - 10.3 定制操作 - 10.3.1 向算法传递函数
 * Effective+C++中文版（第三版）：条款31 千万不要返回局部对象的引用，或指向它们的指针
 */
#include <iostream>
#include <functional>

class BatteryMonitor {
public:
    using ShutdownCallback = std::function<void()>;
    void setBatteryShutdownCallback(ShutdownCallback callback) {
        shutdown_callback = callback;
    }
    void simulateLowBattery() {
        if (shutdown_callback) {
            shutdown_callback();
        }
    }
private:
    ShutdownCallback shutdown_callback;
};

class Speaker {
public:
    void showLowPower() {
        std::cout << "Low power warning!" << std::endl;
    }
};

int main() {
    Speaker speaker;
    BatteryMonitor battery_monitor;
    
    // Lambda表达式作为回调函数
    battery_monitor.setBatteryShutdownCallback([&speaker]() {
        speaker.showLowPower();
    });
    
    battery_monitor.simulateLowBattery();
    return 0;
}
