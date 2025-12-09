/*
 * 知识点：命名空间
 * 源项目：e:\3.esp-brookesia\esp-brookesia\core\brookesia_core\esp_brookesia.hpp 第15-19行
 * C++ Primer中文版（第五版）：第3章 字符串、向量和数组 - 3.1 命名空间的using声明
 * Effective+C++中文版（第三版）：无直接对应章节
 */

#include <iostream>

// 定义命名空间
namespace esp_brookesia
{
    namespace systems
    {
        namespace base
        {
            // 命名空间内的类
            class App
            {
            public:
                void run()
                {
                    std::cout << "App running" << std::endl;
                }
            };
        }
    }
}

// 使用命名空间
typedef esp_brookesia::systems::base::App BaseApp;

int main()
{
    BaseApp app;
    app.run();
    return 0;
}