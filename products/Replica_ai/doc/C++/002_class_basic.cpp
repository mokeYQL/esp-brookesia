/*
 * 知识点：类的基本概念
 * 源项目：e:\3.esp-brookesia\esp-brookesia\products\speaker\main\modules	ouch_sensor.h 第13-26行
 * C++ Primer中文版（第五版）：第7章 类 - 7.1 定义抽象数据类型
 * Effective+C++中文版（第三版）：条款1 视C++为一个语言联邦
 */

#include <iostream>
#include <string>

// 基本类定义
class TouchSensor
{
public:
    // 构造函数
    TouchSensor()
    {
        std::cout << "TouchSensor constructed" << std::endl;
    }

    // 析构函数
    ~TouchSensor()
    {
        std::cout << "TouchSensor destructed" << std::endl;
    }

    // 成员函数
    bool init()
    {
        std::cout << "TouchSensor initialized" << std::endl;
        return true;
    }

    // 成员函数带参数和返回值
    int getTouchValue(int channel)
    {
        return channel * 100;
    }

private:
    // 私有成员变量
    int _min = 0;
    int _max = 1023;
    std::string _name = "Touch Sensor";
};

int main()
{
    TouchSensor sensor;
    sensor.init();
    int value = sensor.getTouchValue(1);
    std::cout << "Touch value: " << value << std::endl;
    return 0;
}