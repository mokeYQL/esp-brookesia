/*
 * 知识点：C++17替代方案（替代C++20 Concepts）
 * 源项目：e:\3.esp-brookesia\esp-brookesia\products\speaker\main\modules\system.cpp 第125-126行
 * C++ Primer中文版（第五版）：第16章 模板与泛型编程 - 16.1 定义模板
 * Effective+C++中文版（第三版）：条款41 了解隐式接口和编译期多态
 */
#include <iostream>
#include <string>
#include <memory>
#include <vector>

// 基础应用类
class App {
public:
    App(const std::string& name, int id) : name(name), id(id) {}
    virtual ~App() = default;
    
    std::string getName() const { return name; }
    int getId() const { return id; }
    
    virtual bool initialize() = 0;
    virtual bool start() = 0;
    virtual bool stop() = 0;
    
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

// 演示打印功能（C++17版本，不使用concepts）
template <typename T>
void print_item(const T& item) {
    std::cout << "Printing: " << item << std::endl;
}

// 演示应用管理功能（C++17版本，不使用concepts）
void manage_app(App& app) {
    std::cout << "=== Managing App: " << app.getName() << " (ID: " << app.getId() << ") ===" << std::endl;
    
    if (app.initialize()) {
        std::cout << "✓ App initialized successfully" << std::endl;
        
        if (app.start()) {
            std::cout << "✓ App started successfully" << std::endl;
            
            // 运行一段时间后停止
            std::cout << "App is running..." << std::endl;
            
            if (app.stop()) {
                std::cout << "✓ App stopped successfully" << std::endl;
            } else {
                std::cout << "✗ Failed to stop app" << std::endl;
            }
        } else {
            std::cout << "✗ Failed to start app" << std::endl;
        }
    } else {
        std::cout << "✗ Failed to initialize app" << std::endl;
    }
}

// 演示模板类（C++17版本，不使用concepts）
template <typename AppT>
class AppManager {
public:
    void add_app(std::unique_ptr<AppT> app) {
        apps.push_back(std::move(app));
    }
    
    void manage_all() {
        for (auto& app : apps) {
            manage_app(*app);
        }
    }
    
private:
    std::vector<std::unique_ptr<AppT>> apps;
};

int main() {
    std::cout << "=== Testing C++17 Alternative to Concepts ===" << std::endl;
    
    // 演示打印功能
    std::cout << "\n1. Testing print_item:" << std::endl;
    print_item(42);
    print_item(3.14);
    print_item("Hello, C++17!");
    print_item(std::string("String object"));
    
    // 演示应用管理
    std::cout << "\n2. Testing app management:" << std::endl;
    
    SettingsApp settings_app(1);
    manage_app(settings_app);
    
    CalculatorApp calc_app(2);
    manage_app(calc_app);
    
    // 演示模板类
    std::cout << "\n3. Testing AppManager with templates:" << std::endl;
    
    AppManager<App> app_manager;
    app_manager.add_app(std::make_unique<SettingsApp>(3));
    app_manager.add_app(std::make_unique<CalculatorApp>(4));
    
    app_manager.manage_all();
    
    std::cout << "\nAll tests completed successfully!" << std::endl;
    
    return 0;
}
