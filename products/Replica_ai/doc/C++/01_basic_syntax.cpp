// ============================================================================//
// ESP-Brookesia 项目 C++ 学习文件 01_basic_syntax.cpp
// 基础语法特性：命名空间、类与对象、构造函数、访问控制、结构体、类型别名
// ============================================================================//

#include <iostream>
#include <string>

// =========================================
// 1. 命名空间（Namespace）
// =========================================
// 命名空间用于组织代码，避免名称冲突
// 学习要点：命名空间的定义、使用、嵌套命名空间、using声明

// 示例1：基本命名空间定义
namespace esp_brookesia
{
    namespace systems
    {
        namespace base
        {
            // 嵌套命名空间
            int version = 100;

            void print_version()
            {
                std::cout << "ESP-Brookesia Base System Version: " << version << std::endl;
            }
        }
    }
}

// 示例2：using声明和using指令
using esp_brookesia::systems::base::print_version;
using std::cout;
using std::endl;

// 示例3：匿名命名空间（仅在当前文件可见）
namespace
{
    const int MAX_BUFFER_SIZE = 1024;

    void private_function()
    {
        cout << "This is a private function in anonymous namespace" << endl;
    }
}

// =========================================
// 2. 类与对象（Class and Object）
// =========================================
// 类是C++的核心概念，用于定义抽象数据类型
// 学习要点：类的定义、成员变量、成员函数、对象创建

// 示例1：基础类定义
class TouchSensor
{
private:
    // 私有成员：仅类内部可访问
    int _pin;
    bool _is_pressed;
    std::string _name;

public:
    // 公共成员：外部可访问

    // 示例2：构造函数（Constructor）
    // 默认构造函数
    TouchSensor()
    {
        _pin = 0;
        _is_pressed = false;
        _name = "unknown";
        cout << "TouchSensor default constructor called" << endl;
    }

    // 带参数的构造函数
    TouchSensor(int pin, const std::string &name)
    {
        _pin = pin;
        _is_pressed = false;
        _name = name;
        cout << "TouchSensor constructor called with pin=" << pin << ", name=" << name << endl;
    }

    // 示例3：构造函数初始化列表（Initializer List）
    TouchSensor(int pin, const std::string &name, bool initial_state)
        : _pin(pin), _name(name), _is_pressed(initial_state)
    {
        // 初始化列表方式更高效，尤其是对于const和引用成员
        cout << "TouchSensor constructor with initializer list called" << endl;
    }

    // 示例4：成员函数
    void press()
    {
        _is_pressed = true;
        cout << _name << " (pin " << _pin << ") pressed" << endl;
    }

    void release()
    {
        _is_pressed = false;
        cout << _name << " (pin " << _pin << ") released" << endl;
    }

    bool is_pressed() const
    {
        // const成员函数：不会修改对象状态
        return _is_pressed;
    }

    // 示例5：访问器（Getter）和修改器（Setter）
    int get_pin() const
    {
        return _pin;
    }

    void set_name(const std::string &new_name)
    {
        _name = new_name;
    }

    const std::string &get_name() const
    {
        return _name;
    }
};

// =========================================
// 3. 访问控制（Access Control）
// =========================================
// 学习要点：public、protected、private的区别和使用场景

class AccessControlExample
{
private:
    // private：仅类内部可访问
    // 用于实现封装，隐藏内部实现细节
    int _private_data;

protected:
    // protected：类内部和派生类可访问
    // 用于实现继承中的数据共享
    int _protected_data;

public:
    // public：外部可访问
    // 用于定义类的接口
    int _public_data;

    AccessControlExample() : _private_data(0), _protected_data(1), _public_data(2) {}

    void access_members()
    {
        // 类内部可以访问所有成员
        _private_data = 10;
        _protected_data = 20;
        _public_data = 30;
    }

    int get_private_data() const
    {
        // 提供公共接口访问私有数据
        return _private_data;
    }
};

// =========================================
// 4. 结构体（Struct）
// =========================================
// 结构体与类的区别：默认访问控制为public
// 学习要点：结构体的定义、使用场景

// 示例1：基础结构体
struct Point
{
    // 默认public
    int x;
    int y;

    // 结构体也可以有成员函数
    void print() const
    {
        cout << "Point(" << x << ", " << y << ")" << endl;
    }

    // 结构体也可以有构造函数
    Point() : x(0), y(0) {}
    Point(int x_val, int y_val) : x(x_val), y(y_val) {}
};

// 示例2：结构体用于配置信息
struct Config
{
    std::string name;
    int version;
    bool debug_mode;
    int max_connections;

    // 静态成员函数：用于简化对象创建
    static Config SIMPLE_CONSTRUCTOR()
    {
        Config cfg;
        cfg.name = "default";
        cfg.version = 1;
        cfg.debug_mode = false;
        cfg.max_connections = 10;
        return cfg;
    }
};

// =========================================
// 5. 类型别名（Type Alias）
// =========================================
// 学习要点：using和typedef的使用

// 示例1：typedef方式
typedef unsigned int uint32;
typedef void (*CallbackFunc)(int, const std::string &);

// 示例2：using方式（C++11推荐）
using uint64 = unsigned long long;
using StringVector = std::vector<std::string>;
using ConfigPtr = Config *;

// =========================================
// 6. 条件编译（Conditional Compilation）
// =========================================
// 学习要点：#ifdef、#ifndef、#define、#endif、#else

// 示例：根据不同平台编译不同代码
#define ESP_PLATFORM

#ifdef ESP_PLATFORM
#define PLATFORM_NAME "ESP32"
#else
#define PLATFORM_NAME "Unknown"
#endif

// =========================================
// 7. 外部C链接（Extern "C"）
// =========================================
// 用于C++与C代码的互操作

// 示例：包含C头文件时使用extern "C"
extern "C"
{
    // 模拟C头文件中的函数声明
    int c_function(int a, int b);

    // 实际实现
    int c_function(int a, int b)
    {
        return a + b;
    }
}

// =========================================
// 主函数：测试所有特性
// =========================================
int main()
{
    cout << "=========================================" << endl;
    cout << "ESP-Brookesia C++ 基础语法学习" << endl;
    cout << "=========================================" << endl;

    // =========================================
    // 测试1：命名空间
    cout << "\n1. 测试命名空间：" << endl;
    cout << "-----------------------------------------" << endl;
    esp_brookesia::systems::base::print_version();
    print_version(); // 使用using声明
    private_function(); // 匿名命名空间函数
    
    // =========================================
    // 测试2：类与对象
    cout << "\n2. 测试类与对象：" << endl;
    cout << "-----------------------------------------" << endl;
    
    // 测试构造函数
    TouchSensor sensor1; // 默认构造函数
    TouchSensor sensor2(4, "Button1"); // 带参数构造函数
    TouchSensor sensor3(5, "Button2", true); // 初始化列表构造函数
    
    // 测试成员函数
    sensor1.press();
    sensor2.release();
    cout << "sensor3 is pressed: " << (sensor3.is_pressed() ? "true" : "false") << endl;
    
    // 测试访问器和修改器
    sensor1.set_name("DefaultButton");
    cout << "sensor1 name: " << sensor1.get_name() << endl;
    cout << "sensor1 pin: " << sensor1.get_pin() << endl;
    
    // =========================================
    // 测试3：访问控制
    cout << "\n3. 测试访问控制：" << endl;
    cout << "-----------------------------------------" << endl;
    AccessControlExample access_obj;
    access_obj._public_data = 100; // 可以直接访问public成员
    // access_obj._private_data = 200; // 错误：private成员不可访问
    // access_obj._protected_data = 300; // 错误：protected成员不可访问
    access_obj.access_members(); // 通过成员函数访问私有成员
    cout << "private_data via getter: " << access_obj.get_private_data() << endl;
    cout << "public_data: " << access_obj._public_data << endl;
    
    // =========================================
    // 测试4：结构体
    cout << "\n4. 测试结构体：" << endl;
    cout << "-----------------------------------------" << endl;
    
    // Point结构体测试
    Point p1;
    Point p2(10, 20);
    p1.print();
    p2.print();
    
    // Config结构体测试
    Config cfg1;
    cfg1.name = "test_config";
    cfg1.version = 2;
    cfg1.debug_mode = true;
    cfg1.max_connections = 20;
    cout << "Config: " << cfg1.name << ", v" << cfg1.version << endl;
    
    // 使用静态成员函数创建配置
    Config cfg2 = Config::SIMPLE_CONSTRUCTOR();
    cout << "Simple Config: " << cfg2.name << ", v" << cfg2.version << endl;
    
    // =========================================
    // 测试5：类型别名
    cout << "\n5. 测试类型别名：" << endl;
    cout << "-----------------------------------------" << endl;
    uint32 u32 = 42;
    uint64 u64 = 1234567890;
    cout << "uint32: " << u32 << endl;
    cout << "uint64: " << u64 << endl;
    
    // 使用ConfigPtr类型别名
    ConfigPtr cfg_ptr = &cfg1;
    cout << "ConfigPtr points to: " << cfg_ptr->name << endl;
    
    // =========================================
    // 测试6：条件编译
    cout << "\n6. 测试条件编译：" << endl;
    cout << "-----------------------------------------" << endl;
    cout << "Platform: " << PLATFORM_NAME << endl;
    
    // =========================================
    // 测试7：外部C链接
    cout << "\n7. 测试外部C链接：" << endl;
    cout << "-----------------------------------------" << endl;
    int result = c_function(10, 20);
    cout << "C function result: " << result << endl;
    
    cout << "\n=========================================" << endl;
    cout << "基础语法学习完成！" << endl;
    cout << "=========================================" << endl;
    
    return 0;
}

// ============================================================================//
// 代码学习要点总结
// ============================================================================//
/*
1. 命名空间：
   - 用于组织代码，避免名称冲突
   - 支持嵌套命名空间
   - using声明可以简化命名空间的使用
   - 匿名命名空间中的内容仅在当前文件可见

2. 类与对象：
   - 类是抽象数据类型的定义
   - 对象是类的实例
   - 成员变量存储状态，成员函数定义行为
   - 构造函数用于对象初始化

3. 构造函数：
   - 默认构造函数：无参数
   - 带参数构造函数：可以初始化成员变量
   - 初始化列表：更高效的初始化方式，尤其适合const和引用成员

4. 访问控制：
   - public：外部可访问，用于定义类的接口
   - protected：类内部和派生类可访问
   - private：仅类内部可访问，用于封装内部实现

5. 结构体：
   - 与类类似，但默认访问控制为public
   - 常用于组织简单数据
   - 也可以包含成员函数和构造函数

6. 类型别名：
   - typedef：传统方式
   - using：C++11推荐方式，语法更清晰
   - 用于简化复杂类型的使用

7. 条件编译：
   - 用于根据不同条件编译不同代码
   - 常用于跨平台开发
   - 支持#ifdef、#ifndef、#else等指令

8. 外部C链接：
   - 用于C++与C代码的互操作
   - 使用extern "C"声明C函数
*/
