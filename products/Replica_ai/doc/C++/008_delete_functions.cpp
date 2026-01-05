/*
 * 知识点：删除函数（= delete）
 * 源项目：e:\3.esp-brookesia\esp-brookesia\core\brookesia_core\systems\base\esp_brookesia_base_app.hpp 第92-95行
 * C++ Primer中文版（第五版）：第7章 类 - 7.6 类的其他特性
 * Effective+C++中文版（第三版）：条款11 在operator=中处理自我赋值
 */

#include <iostream>
#include <string>
#include <memory>

class NonCopyableApp
{
public:
    NonCopyableApp(const std::string &name) : _name(name)
    {
        std::cout << "NonCopyableApp " << _name << " created" << std::endl;
    }

    ~NonCopyableApp()
    {
        std::cout << "NonCopyableApp " << _name << " destroyed" << std::endl;
    }

    // 删除拷贝构造函数：禁止对象拷贝
    NonCopyableApp(const NonCopyableApp &) = delete;

    // 删除拷贝赋值运算符：禁止对象赋值
    NonCopyableApp &operator=(const NonCopyableApp &) = delete;

    // 删除移动构造函数：禁止对象移动
    NonCopyableApp(NonCopyableApp &&) = delete;

    // 删除移动赋值运算符：禁止对象移动赋值
    NonCopyableApp &operator=(NonCopyableApp &&) = delete;

    void run()
    {
        std::cout << "NonCopyableApp " << _name << " running" << std::endl;
    }

    std::string getName() const
    {
        return _name;
    }

private:
    std::string _name;
};

class CopyableApp
{
public:
    CopyableApp(const std::string &name) : _name(name)
    {
        std::cout << "CopyableApp " << _name << " created" << std::endl;
    }

    ~CopyableApp()
    {
        std::cout << "CopyableApp " << _name << " destroyed" << std::endl;
    }

    // 默认拷贝构造函数（编译器自动生成）
    // CopyableApp(const CopyableApp&) = default;

    // 默认拷贝赋值运算符（编译器自动生成）
    // CopyableApp& operator=(const CopyableApp&) = default;

    void run()
    {
        std::cout << "CopyableApp " << _name << " running" << std::endl;
    }

    std::string getName() const
    {
        return _name;
    }

private:
    std::string _name;
};

// 测试函数：尝试拷贝对象
void testCopy()
{
    std::cout << "\n=== Testing CopyableApp ===" << std::endl;

    // 创建可拷贝对象
    CopyableApp app1("Test1");

    // 拷贝构造
    CopyableApp app2 = app1; // 成功

    // 拷贝赋值
    CopyableApp app3("Test3");
    app3 = app1; // 成功

    std::cout << "CopyableApp test completed" << std::endl;
}

// 测试函数：尝试拷贝不可拷贝对象（会编译失败）
void testNonCopyable()
{
    std::cout << "\n=== Testing NonCopyableApp ===" << std::endl;

    // 创建不可拷贝对象
    NonCopyableApp app1("Test1");
    app1.run();

    /* 以下代码会编译失败，因为拷贝构造和拷贝赋值被删除 */
    // NonCopyableApp app2 = app1;  // 编译错误：拷贝构造被删除
    // NonCopyableApp app3("Test3");
    // app3 = app1;  // 编译错误：拷贝赋值被删除

    std::cout << "NonCopyableApp test completed" << std::endl;
}

// 智能指针测试：不可拷贝对象可以被智能指针管理
void testSmartPointer()
{
    std::cout << "\n=== Testing Smart Pointer with NonCopyableApp ===" << std::endl;

    // 使用智能指针管理不可拷贝对象
    auto app_ptr = std::make_unique<NonCopyableApp>("SmartPointerApp");
    app_ptr->run();

    // 智能指针可以转移所有权（使用std::move）
    auto app_ptr2 = std::move(app_ptr);
    if (app_ptr == nullptr)
    {
        std::cout << "Original smart pointer is now null" << std::endl;
    }
    app_ptr2->run();

    std::cout << "Smart pointer test completed" << std::endl;
}

int main()
{
    testCopy();
    testNonCopyable();
    testSmartPointer();

    return 0;
}