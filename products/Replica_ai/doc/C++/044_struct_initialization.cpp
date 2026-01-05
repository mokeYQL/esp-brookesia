/*
 * 知识点：结构体初始化与成员访问
 * 源项目：e:\3.esp-brookesia\esp-brookesia\products\speaker\main\modules\system.cpp 第237-241行
 * C++ Primer中文版（第五版）：第2章 变量和基本类型 - 2.6 自定义数据结构
 * Effective+C++中文版（第三版）：无直接对应章节
 */
#include <iostream>
#include <string>

// 命名空间嵌套
namespace systems {
    namespace base {
        class Context {
        public:
            enum class AppEventType {
                START,
                STOP,
                RESTART,
                OPERATION
            };
            
            struct AppEventData {
                int id;
                AppEventType type;
                void* data;
            };
        };
    }
}

// 应用管理器类
class AppManager {
public:
    // 检查应用ID是否有效
    bool checkAppID_Valid(int app_id) {
        return app_id > 0 && app_id < 100;
    }
    
    // 发送应用事件
    bool sendAppEvent(const systems::base::Context::AppEventData* event_data) {
        if (!event_data) {
            std::cout << "Invalid event data" << std::endl;
            return false;
        }
        
        std::cout << "Sending app event:" << std::endl;
        std::cout << "  ID: " << event_data->id << std::endl;
        
        switch (event_data->type) {
            case systems::base::Context::AppEventType::START:
                std::cout << "  Type: START" << std::endl;
                break;
            case systems::base::Context::AppEventType::STOP:
                std::cout << "  Type: STOP" << std::endl;
                break;
            case systems::base::Context::AppEventType::RESTART:
                std::cout << "  Type: RESTART" << std::endl;
                break;
            case systems::base::Context::AppEventType::OPERATION:
                std::cout << "  Type: OPERATION" << std::endl;
                break;
            default:
                std::cout << "  Type: UNKNOWN" << std::endl;
                break;
        }
        
        std::cout << "  Data: " << event_data->data << std::endl;
        
        return true;
    }
};

// 应用类
class Speaker {
public:
    AppManager& getManager() {
        return app_manager;
    }
    
    bool checkAppID_Valid(int app_id) {
        return app_manager.checkAppID_Valid(app_id);
    }
    
    bool sendAppEvent(const systems::base::Context::AppEventData* event_data) {
        return app_manager.sendAppEvent(event_data);
    }
    
private:
    AppManager app_manager;
};

// 应用操作数据
struct AppOperationData {
    int code;
    int payload;
};

int main() {
    Speaker speaker;
    
    // 初始化结构体
    systems::base::Context::AppEventData event_data = {
        .id = 5,
        .type = systems::base::Context::AppEventType::START,
        .data = nullptr
    };
    
    std::cout << "=== Testing Struct Initialization and Access ===" << std::endl;
    
    // 检查应用ID是否有效
    if (speaker.checkAppID_Valid(event_data.id)) {
        std::cout << "App ID " << event_data.id << " is valid" << std::endl;
        
        // 发送应用事件
        speaker.sendAppEvent(&event_data);
    } else {
        std::cout << "App ID " << event_data.id << " is invalid" << std::endl;
    }
    
    // 测试带数据的事件
    std::cout << "\n=== Testing Event with Data ===" << std::endl;
    AppOperationData operation_data = {
        .code = 100,
        .payload = 200
    };
    
    systems::base::Context::AppEventData operation_event_data = {
        .id = 5,
        .type = systems::base::Context::AppEventType::OPERATION,
        .data = &operation_data
    };
    
    speaker.sendAppEvent(&operation_event_data);
    
    return 0;
}
