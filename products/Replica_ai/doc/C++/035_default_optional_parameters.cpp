/*
 * 知识点：函数默认参数与可选参数
 * 源项目：e:\3.esp-brookesia\esp-brookesia\products\speaker\main\modules\system.cpp 第196-197行
 * C++ Primer中文版（第五版）：第6章 函数 - 6.5 特殊用途语言特性 - 6.5.1 默认实参
 * Effective+C++中文版（第三版）：条款36 绝不重新定义继承而来的non-virtual函数
 */
#include <iostream>
#include <vector>
#include <string>
#include <optional>

class Speaker {
public:
    struct AppInfo {
        std::string name;
        int id;
    };
    
    // 带默认参数的函数
    bool installAppFromRegistry(const std::vector<AppInfo>& inited_apps, 
                               const std::vector<std::string>* ordered_app_names = nullptr) {
        std::cout << "Installing apps..." << std::endl;
        
        if (ordered_app_names) {
            std::cout << "Using ordered app names:" << std::endl;
            for (const auto& name : *ordered_app_names) {
                std::cout << "  - " << name << std::endl;
            }
        } else {
            std::cout << "Using default order:" << std::endl;
            for (const auto& app : inited_apps) {
                std::cout << "  - " << app.name << std::endl;
            }
        }
        return true;
    }
    
    // 使用std::optional的函数
    bool configureSpeaker(const std::string& config, 
                         std::optional<int> volume = std::nullopt, 
                         std::optional<bool> mute = std::nullopt) {
        std::cout << "Configuring speaker with: " << config << std::endl;
        
        if (volume.has_value()) {
            std::cout << "Setting volume to: " << volume.value() << std::endl;
        }
        
        if (mute.has_value()) {
            std::cout << "Setting mute to: " << (mute.value() ? "true" : "false") << std::endl;
        }
        
        return true;
    }
};

int main() {
    Speaker speaker;
    
    std::vector<Speaker::AppInfo> apps = {
        {"Settings", 1},
        {"Calculator", 2},
        {"Timer", 3}
    };
    
    // 测试默认参数（不传递第二个参数）
    std::cout << "=== Testing default parameter ===" << std::endl;
    speaker.installAppFromRegistry(apps);
    
    // 测试传递所有参数
    std::cout << "\n=== Testing all parameters ===" << std::endl;
    std::vector<std::string> ordered_names = {"Calculator", "Timer", "Settings"};
    speaker.installAppFromRegistry(apps, &ordered_names);
    
    // 测试std::optional参数
    std::cout << "\n=== Testing std::optional parameters ===" << std::endl;
    
    // 只传递必要参数
    speaker.configureSpeaker("basic_config");
    
    // 传递一个可选参数
    speaker.configureSpeaker("volume_config", 80);
    
    // 传递所有参数
    speaker.configureSpeaker("full_config", 60, true);
    
    // 传递第二个可选参数，跳过第一个
    speaker.configureSpeaker("mute_config", std::nullopt, false);
    
    return 0;
}
