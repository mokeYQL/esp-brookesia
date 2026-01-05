/*
 * 知识点：类型别名
 * 源项目：e:\3.esp-brookesia\esp-brookesia\core\brookesia_core\esp_brookesia.hpp 第184-195行
 * C++ Primer中文版（第五版）：第2章 变量和基本类型 - 2.5 处理类型 - 2.5.1 类型别名
 * Effective+C++中文版（第三版）：条款9 绝不在构造和析构过程中调用virtual函数
 */

#include <iostream>
#include <string>
#include <vector>
#include <memory>

// 1. 使用typedef定义类型别名（传统方式）
typedef int AppId;
typedef std::string AppName;
typedef std::vector<AppId> AppIdList;

typedef struct
{
    int x;
    int y;
} Point2D;

// 2. 使用using定义类型别名（C++11及以上，推荐）
using ModernAppId = int;
using ModernAppName = std::string;
using ModernAppIdList = std::vector<ModernAppId>;

// 3. 模板类型别名（已注释，因为typedef不能用于模板，使用第4种方式）
// template<typename T>
// typedef std::vector<T> Vector;

// 4. C++11模板别名（推荐）
template <typename T>
using ModernVector = std::vector<T>;

// 5. 智能指针类型别名
template <typename T>
using SharedPtr = std::shared_ptr<T>;

template <typename T>
using UniquePtr = std::unique_ptr<T>;

// 6. 函数指针类型别名
typedef void (*AppCallback)(AppId, const char *);
using ModernAppCallback = void (*)(ModernAppId, const char *);

// 7. 常量类型别名
typedef const char *ConstString;
using ModernConstString = const char *;

class App
{
public:
    // 使用类型别名作为成员变量类型
    App(AppId id, const AppName &name) : _id(id), _name(name)
    {
        std::cout << "App created: ID=" << _id << ", Name=" << _name << std::endl;
    }

    ~App()
    {
        std::cout << "App destroyed: ID=" << _id << ", Name=" << _name << std::endl;
    }

    // 使用类型别名作为函数参数和返回值
    AppId getId() const
    {
        return _id;
    }

    AppName getName() const
    {
        return _name;
    }

    // 使用智能指针类型别名
    static SharedPtr<App> create(AppId id, const AppName &name)
    {
        return std::make_shared<App>(id, name);
    }

private:
    AppId _id;
    AppName _name;
};

// 使用函数指针类型别名
void appCallback(AppId id, const char *message)
{
    std::cout << "Callback for App " << id << ": " << message << std::endl;
}

void modernAppCallback(ModernAppId id, const char *message)
{
    std::cout << "Modern Callback for App " << id << ": " << message << std::endl;
}

int main()
{
    std::cout << "=== Type Aliases Examples ===" << std::endl;

    // 1. 使用typedef类型别名
    AppId appId = 1001;
    AppName appName = "Calculator";
    AppIdList appIds = {1001, 1002, 1003};

    std::cout << "\n1. typedef Usage:" << std::endl;
    std::cout << "App ID: " << appId << std::endl;
    std::cout << "App Name: " << appName << std::endl;
    std::cout << "App IDs: ";
    for (auto id : appIds)
    {
        std::cout << id << " ";
    }
    std::cout << std::endl;

    // 2. 使用using类型别名
    ModernAppId modernAppId = 2001;
    ModernAppName modernAppName = "Game 2048";
    ModernAppIdList modernAppIds = {2001, 2002, 2003};

    std::cout << "\n2. using Usage:" << std::endl;
    std::cout << "Modern App ID: " << modernAppId << std::endl;
    std::cout << "Modern App Name: " << modernAppName << std::endl;
    std::cout << "Modern App IDs: ";
    for (auto id : modernAppIds)
    {
        std::cout << id << " ";
    }
    std::cout << std::endl;

    // 3. 使用模板类型别名
    ModernVector<int> intVector = {1, 2, 3, 4, 5};
    ModernVector<std::string> stringVector = {"apple", "banana", "cherry"};

    std::cout << "\n3. Template Type Aliases:" << std::endl;
    std::cout << "intVector size: " << intVector.size() << std::endl;
    std::cout << "stringVector size: " << stringVector.size() << std::endl;

    // 4. 使用智能指针类型别名
    auto app1 = App::create(1001, "Calculator");
    SharedPtr<App> app2 = std::make_shared<App>(1002, "Game 2048");
    UniquePtr<App> app3 = std::make_unique<App>(1003, "Settings");

    std::cout << "\n4. Smart Pointer Type Aliases:" << std::endl;
    std::cout << "app1: " << app1->getName() << std::endl;
    std::cout << "app2: " << app2->getName() << std::endl;
    std::cout << "app3: " << app3->getName() << std::endl;

    // 5. 使用函数指针类型别名
    AppCallback callback = appCallback;
    callback(appId, "Test message");

    ModernAppCallback modernCallback = modernAppCallback;
    modernCallback(modernAppId, "Modern test message");

    std::cout << "\n5. Function Pointer Type Aliases:" << std::endl;

    // 6. 使用常量类型别名
    ConstString constStr = "Hello, typedef!";
    ModernConstString modernConstStr = "Hello, using!";

    std::cout << "\n6. Const Type Aliases:" << std::endl;
    std::cout << "constStr: " << constStr << std::endl;
    std::cout << "modernConstStr: " << modernConstStr << std::endl;

    return 0;
}