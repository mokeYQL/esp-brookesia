/*
 * 知识点：std::vector动态数组
 * 源项目：e:\3.esp-brookesia\esp-brookesia\products\speaker\main\modules\system.cpp 第195-197行
 * C++ Primer中文版（第五版）：第9章 顺序容器 - 9.2 容器库概览
 * Effective+C++中文版（第三版）：条款14 资源管理类中小心拷贝行为
 */
#include <iostream>
#include <vector>
#include <string>

class Speaker {
public:
    struct AppInfo {
        std::string name;
        int id;
    };
    
    bool installAppFromRegistry(const std::vector<AppInfo>& inited_apps, const std::vector<std::string>* ordered_app_names = nullptr) {
        std::cout << "Installing apps from registry..." << std::endl;
        
        if (ordered_app_names) {
            std::cout << "Using ordered app names:" << std::endl;
            for (const auto& app_name : *ordered_app_names) {
                std::cout << "  - " << app_name << std::endl;
            }
        } else {
            std::cout << "Using default order:" << std::endl;
            for (const auto& app : inited_apps) {
                std::cout << "  - " << app.name << std::endl;
            }
        }
        
        return true;
    }
};

int main() {
    Speaker speaker;
    
    // 初始化应用列表
    std::vector<Speaker::AppInfo> inited_apps = {
        {"Settings", 1},
        {"AI_Profile", 2},
        {"2048", 3},
        {"Calculator", 4},
        {"Timer", 5},
        {"Pos", 6},
        {"UsbdNcm", 7}
    };
    
    // 定义应用安装顺序
    std::vector<std::string> ordered_app_names = {"Settings", "AI_Profile", "2048", "Calculator", "Timer", "Pos", "UsbdNcm"};
    
    // 安装应用
    speaker.installAppFromRegistry(inited_apps, &ordered_app_names);
    
    // 测试vector的其他操作
    std::cout << "\nVector operations:" << std::endl;
    std::cout << "App count: " << inited_apps.size() << std::endl;
    std::cout << "First app: " << inited_apps.front().name << std::endl;
    std::cout << "Last app: " << inited_apps.back().name << std::endl;
    
    // 添加新应用
    inited_apps.push_back({"NewApp", 8});
    std::cout << "After adding new app, count: " << inited_apps.size() << std::endl;
    
    return 0;
}
