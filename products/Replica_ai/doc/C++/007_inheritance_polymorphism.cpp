/*
 * 知识点：继承与多态
 * 源项目：e:\3.esp-brookesia\esp-brookesia\apps\brookesia_app_calculator\esp_brookesia_app_calculator.hpp 第13行
 * C++ Primer中文版（第五版）：第15章 面向对象程序设计 - 15.1 OOP：概述
 * Effective+C++中文版（第三版）：条款32 确定你的public继承塑模出is-a关系
 */

#include <iostream>
#include <string>

// 基类：应用程序基类
class BaseApp {
public:
    BaseApp(const std::string& name) : _name(name) {
        std::cout << "BaseApp " << _name << " created" << std::endl;
    }
    
    // 虚析构函数：确保子类析构函数被正确调用
    virtual ~BaseApp() {
        std::cout << "BaseApp " << _name << " destroyed" << std::endl;
    }
    
    // 纯虚函数：必须在子类中实现
    virtual bool run() = 0;
    
    // 虚函数：可以在子类中重写
    virtual bool back() {
        std::cout << "BaseApp " << _name << " back" << std::endl;
        return true;
    }
    
    // 虚函数：可以在子类中重写
    virtual bool close() {
        std::cout << "BaseApp " << _name << " close" << std::endl;
        return true;
    }
    
    // 普通成员函数
    std::string getName() const {
        return _name;
    }
    
private:
    std::string _name;
};

// 派生类1：计算器应用
class CalculatorApp : public BaseApp {
public:
    CalculatorApp() : BaseApp("Calculator") {
        std::cout << "CalculatorApp created" << std::endl;
    }
    
    ~CalculatorApp() {
        std::cout << "CalculatorApp destroyed" << std::endl;
    }
    
    // 重写纯虚函数
    bool run() override {
        std::cout << "CalculatorApp running" << std::endl;
        return true;
    }
    
    // 重写虚函数
    bool back() override {
        std::cout << "CalculatorApp back" << std::endl;
        return true;
    }
    
    // 派生类特有的成员函数
    void calculate(int a, int b) {
        std::cout << "Calculator result: " << (a + b) << std::endl;
    }
};

// 派生类2：游戏应用
class GameApp : public BaseApp {
public:
    GameApp() : BaseApp("2048 Game") {
        std::cout << "GameApp created" << std::endl;
    }
    
    ~GameApp() {
        std::cout << "GameApp destroyed" << std::endl;
    }
    
    // 重写纯虚函数
    bool run() override {
        std::cout << "GameApp running" << std::endl;
        return true;
    }
    
    // 派生类特有的成员函数
    void startGame() {
        std::cout << "Game started" << std::endl;
    }
};

// 演示多态
void runApp(BaseApp* app) {
    std::cout << "Running app: " << app->getName() << std::endl;
    app->run();
    app->back();
    app->close();
}

int main() {
    // 创建派生类对象
    CalculatorApp calcApp;
    GameApp gameApp;
    
    std::cout << "\n=== Testing direct calls ===" << std::endl;
    calcApp.run();
    calcApp.calculate(10, 20);
    
    gameApp.run();
    gameApp.startGame();
    
    std::cout << "\n=== Testing polymorphism ===" << std::endl;
    // 使用基类指针调用派生类方法，实现多态
    runApp(&calcApp);
    runApp(&gameApp);
    
    std::cout << "\n=== Testing dynamic allocation ===" << std::endl;
    // 动态分配对象，使用基类指针
    BaseApp* dynamicApp = new CalculatorApp();
    dynamicApp->run();
    delete dynamicApp;  // 虚析构函数确保正确清理
    
    return 0;
}