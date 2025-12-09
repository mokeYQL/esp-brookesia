/*
 * 知识点：命名空间using指令
 * 源项目：e:\3.esp-brookesia\esp-brookesia\products\speaker\main\modules\system.cpp 第50-55行
 * C++ Primer中文版（第五版）：第3章 字符串、向量和数组 - 3.1 命名空间的using声明
 * Effective+C++中文版（第三版）：无直接对应章节
 */
#include <iostream>

// 模拟esp_brookesia命名空间结构
namespace esp_brookesia {
    namespace systems {
        namespace speaker {
            class Speaker {
            public:
                void speak() { std::cout << "Speaker::speak()" << std::endl; }
            };
        }
        
        namespace base {
            class Context {
            public:
                void initialize() { std::cout << "Context::initialize()" << std::endl; }
            };
        }
    }
    
    namespace gui {
        class Display {
        public:
            void render() { std::cout << "Display::render()" << std::endl; }
        };
    }
    
    namespace apps {
        class AppManager {
        public:
            void launch() { std::cout << "AppManager::launch()" << std::endl; }
        };
    }
}

int main() {
    // 不使用using指令的情况
    std::cout << "=== Without using directives ===" << std::endl;
    esp_brookesia::systems::speaker::Speaker speaker1;
    speaker1.speak();
    
    esp_brookesia::gui::Display display1;
    display1.render();
    
    // 使用using指令简化代码
    std::cout << "\n=== With using directives ===" << std::endl;
    using namespace esp_brookesia::systems::speaker;
    using namespace esp_brookesia::gui;
    using namespace esp_brookesia::apps;
    
    Speaker speaker2;
    speaker2.speak();
    
    Display display2;
    display2.render();
    
    AppManager app_manager;
    app_manager.launch();
    
    // 可以混合使用
    std::cout << "\n=== Mixed usage ===" << std::endl;
    esp_brookesia::systems::base::Context context;
    context.initialize();
    
    return 0;
}
