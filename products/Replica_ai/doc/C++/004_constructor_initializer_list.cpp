/*
 * 知识点：构造函数与初始化列表
 * 源项目：e:\3.esp-brookesia\esp-brookesia\apps\brookesia_app_calculator\esp_brookesia_app_calculator.cpp 第47-65行
 * C++ Primer中文版（第五版）：第7章 类 - 7.5 构造函数再探
 * Effective+C++中文版（第三版）：条款4 确定对象被使用前已先被初始化
 */

#include <iostream>
#include <string>
#include <vector>

class AppConfig {
public:
    std::string name;
    std::string icon;
    int screenWidth;
    int screenHeight;
    bool enableDefaultScreen;
};

class BaseApp {
public:
    // 构造函数1：默认构造
    BaseApp() : _name("Default"), _icon("default.png"), _screenWidth(320), _screenHeight(240) {
        std::cout << "BaseApp default constructor" << std::endl;
    }
    
    // 构造函数2：带参数构造
    BaseApp(const std::string& name, const std::string& icon) 
        : _name(name), _icon(icon), _screenWidth(320), _screenHeight(240) {
        std::cout << "BaseApp constructor with name and icon" << std::endl;
    }
    
    // 构造函数3：使用结构体初始化列表
    BaseApp(const AppConfig& config) 
        : _name(config.name), 
          _icon(config.icon), 
          _screenWidth(config.screenWidth), 
          _screenHeight(config.screenHeight),
          _enableDefaultScreen(config.enableDefaultScreen),
          _resourceCount(0) {
        std::cout << "BaseApp constructor with AppConfig" << std::endl;
    }
    
    // 禁用拷贝构造
    BaseApp(const BaseApp&) = delete;
    BaseApp& operator=(const BaseApp&) = delete;
    
    ~BaseApp() {
        std::cout << "BaseApp destructor" << std::endl;
    }
    
    void displayInfo() {
        std::cout << "Name: " << _name << std::endl;
        std::cout << "Icon: " << _icon << std::endl;
        std::cout << "Screen: " << _screenWidth << "x" << _screenHeight << std::endl;
        std::cout << "Enable Default Screen: " << (_enableDefaultScreen ? "true" : "false") << std::endl;
    }
    
private:
    std::string _name;
    std::string _icon;
    int _screenWidth;
    int _screenHeight;
    bool _enableDefaultScreen;
    int _resourceCount;
    std::vector<void*> _resources;
};

int main() {
    // 使用不同构造函数创建对象
    BaseApp app1;
    app1.displayInfo();
    std::cout << "-------------------" << std::endl;
    
    BaseApp app2("Calculator", "calc.png");
    app2.displayInfo();
    std::cout << "-------------------" << std::endl;
    
    AppConfig config = {
        "Weather",
        "weather.png",
        480,
        480,
        true
    };
    BaseApp app3(config);
    app3.displayInfo();
    
    return 0;
}