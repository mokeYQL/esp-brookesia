/*
 * 知识点：虚函数与多态进阶
 * 源项目：e:\3.esp-brookesia\esp-brookesia\products\speaker\main\modules\system.cpp 第125-126行
 * C++ Primer中文版（第五版）：第15章 面向对象程序设计 - 15.3 虚函数 - 15.3.1 虚函数的工作原理
 * Effective+C++中文版（第三版）：条款34 区分接口继承和实现继承
 */
#include <iostream>
#include <memory>
#include <vector>
#include <string>

// 基础应用类
class App {
public:
    App(const std::string& name, int id) : name(name), id(id) {}
    virtual ~App() = default;
    
    // 纯虚函数，必须在派生类中实现
    virtual bool initialize() = 0;
    virtual bool start() = 0;
    virtual bool stop() = 0;
    virtual std::string getAppType() const = 0;
    
    // 普通虚函数，可以在派生类中重写
    virtual void pause() {
        std::cout << name << ": Base pause implementation" << std::endl;
    }
    
    // 非虚函数，派生类不能重写
    int getId() const { return id; }
    std::string getName() const { return name; }
    
private:
    std::string name;
    int id;
};

// 设置应用
class SettingsApp : public App {
public:
    SettingsApp(int id) : App("Settings", id) {}
    
    bool initialize() override {
        std::cout << getName() << ": Initializing settings..." << std::endl;
        return true;
    }
    
    bool start() override {
        std::cout << getName() << ": Starting settings app..." << std::endl;
        return true;
    }
    
    bool stop() override {
        std::cout << getName() << ": Stopping settings app..." << std::endl;
        return true;
    }
    
    std::string getAppType() const override {
        return "Settings";
    }
    
    void pause() override {
        std::cout << getName() << ": Pausing settings... Saving state..." << std::endl;
    }
    
    // 派生类特有的方法
    void openNetworkSettings() {
        std::cout << getName() << ": Opening network settings" << std::endl;
    }
};

// 计算器应用
class CalculatorApp : public App {
public:
    CalculatorApp(int id) : App("Calculator", id) {}
    
    bool initialize() override {
        std::cout << getName() << ": Initializing calculator..." << std::endl;
        return true;
    }
    
    bool start() override {
        std::cout << getName() << ": Starting calculator app..." << std::endl;
        return true;
    }
    
    bool stop() override {
        std::cout << getName() << ": Stopping calculator app..." << std::endl;
        return true;
    }
    
    std::string getAppType() const override {
        return "Calculator";
    }
    
    // 派生类特有的方法
    int calculate(int a, int b, char op) {
        switch (op) {
            case '+': return a + b;
            case '-': return a - b;
            case '*': return a * b;
            case '/': return b != 0 ? a / b : 0;
            default: return 0;
        }
    }
};

// 应用管理器
class AppManager {
public:
    // 添加应用
    void addApp(std::unique_ptr<App> app) {
        apps.push_back(std::move(app));
        std::cout << "Added app: " << apps.back()->getName() << std::endl;
    }
    
    // 初始化所有应用
    void initializeAllApps() {
        std::cout << "\n=== Initializing all apps ===" << std::endl;
        for (auto& app : apps) {
            app->initialize();
        }
    }
    
    // 启动所有应用
    void startAllApps() {
        std::cout << "\n=== Starting all apps ===" << std::endl;
        for (auto& app : apps) {
            app->start();
        }
    }
    
    // 暂停所有应用
    void pauseAllApps() {
        std::cout << "\n=== Pausing all apps ===" << std::endl;
        for (auto& app : apps) {
            app->pause();
        }
    }
    
    // 停止所有应用
    void stopAllApps() {
        std::cout << "\n=== Stopping all apps ===" << std::endl;
        for (auto& app : apps) {
            app->stop();
        }
    }
    
    // 演示动态类型转换
    void demonstrateDynamicCast() {
        std::cout << "\n=== Demonstrating dynamic_cast ===" << std::endl;
        
        for (auto& app : apps) {
            // 检查SettingsApp类型
            if (auto settings_app = dynamic_cast<SettingsApp*>(app.get())) {
                std::cout << "Found SettingsApp: " << app->getName() << std::endl;
                settings_app->openNetworkSettings();
            }
            
            // 检查CalculatorApp类型
            if (auto calc_app = dynamic_cast<CalculatorApp*>(app.get())) {
                std::cout << "Found CalculatorApp: " << app->getName() << std::endl;
                int result = calc_app->calculate(10, 5, '+');
                std::cout << "Calculation result: 10 + 5 = " << result << std::endl;
            }
        }
    }
    
private:
    std::vector<std::unique_ptr<App>> apps;
};

int main() {
    AppManager app_manager;
    
    // 添加不同类型的应用
    app_manager.addApp(std::make_unique<SettingsApp>(1));
    app_manager.addApp(std::make_unique<CalculatorApp>(2));
    app_manager.addApp(std::make_unique<SettingsApp>(3));
    
    // 初始化、启动、暂停、停止所有应用
    app_manager.initializeAllApps();
    app_manager.startAllApps();
    app_manager.pauseAllApps();
    app_manager.stopAllApps();
    
    // 演示动态类型转换
    app_manager.demonstrateDynamicCast();
    
    return 0;
}
