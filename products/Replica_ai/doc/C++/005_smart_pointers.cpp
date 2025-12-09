/*
 * 知识点：智能指针
 * 源项目：e:\3.esp-brookesia\esp-brookesia\core\brookesia_core\systems\base\esp_brookesia_base_context.hpp 第176行
 * C++ Primer中文版（第五版）：第12章 动态内存 - 12.1 动态内存与智能指针
 * Effective+C++中文版（第三版）：条款13 以对象管理资源
 */

#include <iostream>
#include <memory>
#include <string>

class Display
{
public:
    Display(const std::string &name) : _name(name)
    {
        std::cout << "Display " << _name << " created" << std::endl;
    }

    ~Display()
    {
        std::cout << "Display " << _name << " destroyed" << std::endl;
    }

    void render()
    {
        std::cout << "Display " << _name << " rendering" << std::endl;
    }

private:
    std::string _name;
};

class EventObject
{
public:
    EventObject()
    {
        std::cout << "EventObject created" << std::endl;
    }

    ~EventObject()
    {
        std::cout << "EventObject destroyed" << std::endl;
    }

    void emitEvent()
    {
        std::cout << "Event emitted" << std::endl;
    }
};

// 自定义智能指针类型定义
template <typename T>
using SharedPtr = std::shared_ptr<T>;

template <typename T>
using UniquePtr = std::unique_ptr<T>;

class SystemContext
{
public:
    SystemContext()
    {
        // 使用 unique_ptr：独占所有权
        _display = std::make_unique<Display>("Main Display");

        // 使用 shared_ptr：共享所有权
        _eventObj = std::make_shared<EventObject>();

        // 多个 shared_ptr 共享同一个对象
        _eventObj2 = _eventObj;

        std::cout << "SystemContext created" << std::endl;
    }

    ~SystemContext()
    {
        std::cout << "SystemContext destroyed" << std::endl;
        // 智能指针自动管理内存，无需手动释放
    }

    void initialize()
    {
        _display->render();
        _eventObj->emitEvent();

        // 检查 shared_ptr 引用计数
        std::cout << "EventObject ref count: " << _eventObj.use_count() << std::endl;
    }

private:
    UniquePtr<Display> _display;
    SharedPtr<EventObject> _eventObj;
    SharedPtr<EventObject> _eventObj2;

    // 原始指针示例（不推荐，用于对比）
    EventObject *_rawEventObj = nullptr;
};

void testSmartPointers()
{
    std::cout << "=== Testing Smart Pointers ===" << std::endl;

    // 局部作用域，自动释放
    {
        SystemContext context;
        context.initialize();
    }

    std::cout << "=== Smart Pointers Test End ===" << std::endl;
}

int main()
{
    testSmartPointers();
    return 0;
}