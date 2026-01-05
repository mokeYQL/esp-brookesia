/*
 * 知识点：命名空间嵌套与using指令进阶
 * 源项目：e:\3.esp-brookesia\esp-brookesia\products\speaker\main\modules\system.cpp 第50-55行
 * C++ Primer中文版（第五版）：第3章 字符串、向量和数组 - 3.1 命名空间的using声明
 * Effective+C++中文版（第三版）：无直接对应章节
 */
#include <iostream>

// 模拟esp_brookesia复杂命名空间结构
namespace esp_brookesia {
    // 系统相关命名空间
    namespace systems {
        // 基础系统
        namespace base {
            class Manager {
            public:
                void initialize() {
                    std::cout << "esp_brookesia::systems::base::Manager::initialize()" << std::endl;
                }
            };
            
            class Context {
            public:
                void configure() {
                    std::cout << "esp_brookesia::systems::base::Context::configure()" << std::endl;
                }
            };
        }
        
        // 扬声器系统
        namespace speaker {
            class Speaker {
            public:
                void speak() {
                    std::cout << "esp_brookesia::systems::speaker::Speaker::speak()" << std::endl;
                }
            };
        }
    }
    
    // GUI相关命名空间
    namespace gui {
        class Display {
        public:
            void render() {
                std::cout << "esp_brookesia::gui::Display::render()" << std::endl;
            }
        };
    }
    
    // 应用相关命名空间
    namespace apps {
        class AppManager {
        public:
            void launch() {
                std::cout << "esp_brookesia::apps::AppManager::launch()" << std::endl;
            }
        };
    }
    
    // 服务相关命名空间
    namespace services {
        class StorageService {
        public:
            void save() {
                std::cout << "esp_brookesia::services::StorageService::save()" << std::endl;
            }
        };
    }
    
    // AI框架命名空间
    namespace ai_framework {
        class AIEngine {
        public:
            void process() {
                std::cout << "esp_brookesia::ai_framework::AIEngine::process()" << std::endl;
            }
        };
    }
}

// 演示不同的命名空间使用方式
void demonstrate_namespace_usage() {
    std::cout << "=== Testing Namespace Usage ===" << std::endl;
    
    // 1. 完全限定名
    std::cout << "\n1. Fully qualified names:" << std::endl;
    esp_brookesia::systems::base::Manager manager1;
    manager1.initialize();
    
    esp_brookesia::gui::Display display1;
    display1.render();
    
    // 2. 使用using声明
    std::cout << "\n2. Using declarations:" << std::endl;
    using esp_brookesia::systems::speaker::Speaker;
    Speaker speaker2;
    speaker2.speak();
    
    // 3. 使用using指令
    std::cout << "\n3. Using directives:" << std::endl;
    using namespace esp_brookesia::apps;
    AppManager app_manager;
    app_manager.launch();
    
    // 4. 嵌套using指令
    std::cout << "\n4. Nested using directives:" << std::endl;
    {
        // 在局部作用域中使用using指令
        using namespace esp_brookesia::services;
        using namespace esp_brookesia::ai_framework;
        
        StorageService storage;
        storage.save();
        
        AIEngine ai_engine;
        ai_engine.process();
    }
    
    // 5. 组合使用
    std::cout << "\n5. Combined usage:" << std::endl;
    using namespace esp_brookesia;
    
    systems::base::Context context;
    context.configure();
    
    gui::Display display2;
    display2.render();
}

int main() {
    demonstrate_namespace_usage();
    return 0;
}
