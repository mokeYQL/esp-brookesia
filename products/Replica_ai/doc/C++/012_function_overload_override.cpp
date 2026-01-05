/*
 * 知识点：函数重载与覆盖
 * 源项目：e:\3.esp-brookesia\esp-brookesia\apps\brookesia_app_calculator\esp_brookesia_app_calculator.hpp 第19-23行
 * C++ Primer中文版（第五版）：第6章 函数 - 6.4 函数重载
 * Effective+C++中文版（第三版）：条款36 绝不重新定义继承而来的non-virtual函数
 */

#include <iostream>
#include <string>
#include <cmath>

class BaseApp {
public:
    BaseApp(const std::string& name) : _name(name) {
        std::cout << "BaseApp " << _name << " created" << std::endl;
    }
    
    virtual ~BaseApp() {
        std::cout << "BaseApp " << _name << " destroyed" << std::endl;
    }
    
    // 虚函数：可以被派生类覆盖
    virtual void run() {
        std::cout << "BaseApp " << _name << " running" << std::endl;
    }
    
    // 虚函数：可以被派生类覆盖
    virtual bool back() {
        std::cout << "BaseApp " << _name << " back" << std::endl;
        return true;
    }
    
    // 函数重载：同名不同参数
    void process(int value) {
        std::cout << "BaseApp processing int: " << value << std::endl;
    }
    
    void process(double value) {
        std::cout << "BaseApp processing double: " << value << std::endl;
    }
    
    void process(const std::string& value) {
        std::cout << "BaseApp processing string: " << value << std::endl;
    }
    
    // 带默认参数的函数
    void configure(int option = 0, const std::string& setting = "default") {
        std::cout << "BaseApp configure: option=" << option << ", setting=" << setting << std::endl;
    }
    
    std::string getName() const {
        return _name;
    }
    
private:
    std::string _name;
};

class CalculatorApp : public BaseApp {
public:
    CalculatorApp() : BaseApp("Calculator") {
        std::cout << "CalculatorApp created" << std::endl;
    }
    
    ~CalculatorApp() {
        std::cout << "CalculatorApp destroyed" << std::endl;
    }
    
    // 函数覆盖：覆盖基类的虚函数
    void run() override {
        std::cout << "CalculatorApp running" << std::endl;
    }
    
    // 函数覆盖：覆盖基类的虚函数
    bool back() override {
        std::cout << "CalculatorApp back" << std::endl;
        return true;
    }
    
    // 函数重载：派生类中重载基类函数
    void calculate(int a, int b) {
        std::cout << "Calculator result (int): " << (a + b) << std::endl;
    }
    
    void calculate(double a, double b) {
        std::cout << "Calculator result (double): " << (a + b) << std::endl;
    }
    
    void calculate(int a, int b, char op) {
        switch (op) {
            case '+':
                std::cout << "Calculator result: " << (a + b) << std::endl;
                break;
            case '-':
                std::cout << "Calculator result: " << (a - b) << std::endl;
                break;
            case '*':
                std::cout << "Calculator result: " << (a * b) << std::endl;
                break;
            case '/':
                if (b != 0) {
                    std::cout << "Calculator result: " << (static_cast<double>(a) / b) << std::endl;
                } else {
                    std::cout << "Calculator error: division by zero" << std::endl;
                }
                break;
            default:
                std::cout << "Calculator error: invalid operator" << std::endl;
        }
    }
    
    // 隐藏基类的同名函数（不使用override）
    void process(int value) {
        std::cout << "CalculatorApp processing int: " << value << std::endl;
    }
};

class GameApp : public BaseApp {
public:
    GameApp() : BaseApp("2048 Game") {
        std::cout << "GameApp created" << std::endl;
    }
    
    ~GameApp() {
        std::cout << "GameApp destroyed" << std::endl;
    }
    
    // 函数覆盖：覆盖基类的虚函数
    void run() override {
        std::cout << "GameApp running" << std::endl;
    }
    
    // 新函数：与基类函数不同名
    void move(int direction) {
        std::string dir_str;
        switch (direction) {
            case 0: dir_str = "up";
 break;
            case 1: dir_str = "right";
 break;
            case 2: dir_str = "down";
 break;
            case 3: dir_str = "left";
 break;
            default: dir_str = "invalid";
        }
        std::cout << "GameApp move: " << dir_str << std::endl;
    }
};

int main() {
    std::cout << "=== Function Overload and Override Examples ===" << std::endl;
    
    // 创建派生类对象
    CalculatorApp calc;
    GameApp game;
    
    std::cout << "\n1. Function Override:" << std::endl;
    calc.run();      // 调用派生类的run()，覆盖基类
    game.run();      // 调用派生类的run()，覆盖基类
    
    std::cout << "\n2. Function Overload in Base Class:" << std::endl;
    // 调用基类的函数重载
    calc.process(10);           // 调用派生类的process()，隐藏了基类的版本
    calc.BaseApp::process(10);  // 显式调用基类的process()
    calc.BaseApp::process(3.14); // 调用基类的double版本
    calc.BaseApp::process("test"); // 调用基类的string版本
    
    std::cout << "\n3. Function Overload in Derived Class:" << std::endl;
    // 调用派生类的函数重载
    calc.calculate(10, 20);           // int版本
    calc.calculate(10.5, 20.5);       // double版本
    calc.calculate(10, 20, '+');      // 带运算符版本
    calc.calculate(10, 20, '*');      // 带运算符版本
    
    std::cout << "\n4. Default Parameters:" << std::endl;
    // 调用带默认参数的函数
    calc.configure();                 // 使用所有默认参数
    calc.configure(1);                // 只提供第一个参数
    calc.configure(2, "custom");      // 提供所有参数
    
    std::cout << "\n5. Polymorphism with Function Override:" << std::endl;
    // 使用基类指针调用派生类函数，实现多态
    BaseApp* app_ptr1 = &calc;
    BaseApp* app_ptr2 = &game;
    
    app_ptr1->run();  // 调用CalculatorApp::run()
    app_ptr2->run();  // 调用GameApp::run()
    
    app_ptr1->back();  // 调用CalculatorApp::back()
    app_ptr2->back();  // 调用BaseApp::back()（GameApp没有覆盖）
    
    return 0;
}