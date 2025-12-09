/*
 * 知识点：C++11特性（auto、范围for、lambda）
 * 源项目：e:\3.esp-brookesia\esp-brookesia\apps\brookesia_app_calculator\esp_brookesia_app_calculator.cpp 第85-100行
 * C++ Primer中文版（第五版）：第11章 关联容器 - 11.2 关联容器操作
 * Effective+C++中文版（第三版）：无直接对应章节
 */

#include <iostream>
#include <string>
#include <vector>
#include <map>
#include <functional>
#include <memory>
#include <algorithm>

// 1. auto 类型推导
void autoTypeDeduction()
{
    std::cout << "\n1. Auto Type Deduction:" << std::endl;

    // 基本类型推导
    auto i = 10;                     // int
    auto d = 3.14;                   // double
    auto s = "hello";                // const char*
    auto str = std::string("world"); // std::string

    std::cout << "i: " << i << ", type: " << typeid(i).name() << std::endl;
    std::cout << "d: " << d << ", type: " << typeid(d).name() << std::endl;
    std::cout << "s: " << s << ", type: " << typeid(s).name() << std::endl;
    std::cout << "str: " << str << ", type: " << typeid(str).name() << std::endl;

    // 容器类型推导
    std::vector<int> vec = {1, 2, 3, 4, 5};
    auto it = vec.begin();          // std::vector<int>::iterator
    auto &ref = vec[0];             // int& (引用)
    const auto &const_ref = vec[1]; // const int& (常量引用)

    std::cout << "*it: " << *it << std::endl;
    ref = 10; // 修改原容器
    std::cout << "vec[0] after modification: " << vec[0] << std::endl;
    std::cout << "const_ref: " << const_ref << std::endl;

    // 函数返回值推导
    auto getValue = []() -> int
    { return 42; };
    auto value = getValue();
    std::cout << "getValue(): " << value << std::endl;
}

// 2. 范围for循环
void rangeBasedForLoop()
{
    std::cout << "\n2. Range-Based For Loop:" << std::endl;

    // 遍历数组
    int arr[] = {1, 2, 3, 4, 5};
    std::cout << "Array elements: ";
    for (auto elem : arr)
    {
        std::cout << elem << " ";
    }
    std::cout << std::endl;

    // 遍历vector
    std::vector<std::string> vec = {"apple", "banana", "cherry"};
    std::cout << "Vector elements: ";
    for (const auto &elem : vec)
    {
        std::cout << elem << " ";
    }
    std::cout << std::endl;

    // 遍历map
    std::map<std::string, int> map = {
        {"apple", 100},
        {"banana", 200},
        {"cherry", 300}};
    std::cout << "Map elements: " << std::endl;
    for (const auto &pair : map)
    {
        std::cout << "  " << pair.first << ": " << pair.second << std::endl;
    }

    // 遍历并修改元素
    std::vector<int> numbers = {1, 2, 3, 4, 5};
    std::cout << "Numbers before modification: ";
    for (const auto &num : numbers)
        std::cout << num << " ";
    std::cout << std::endl;

    for (auto &num : numbers)
    {
        num *= 2;
    }

    std::cout << "Numbers after modification: ";
    for (const auto &num : numbers)
        std::cout << num << " ";
    std::cout << std::endl;
}

// 3. Lambda表达式
void lambdaExpressions()
{
    std::cout << "\n3. Lambda Expressions:" << std::endl;

    // 基本lambda
    auto hello = []()
    {
        std::cout << "Hello from lambda" << std::endl;
    };
    hello();

    // 带参数的lambda
    auto add = [](int a, int b) -> int
    {
        return a + b;
    };
    int sum = add(10, 20);
    std::cout << "add(10, 20) = " << sum << std::endl;

    // 捕获外部变量（值捕获）
    int x = 100;
    auto printX = [x]()
    {
        std::cout << "Captured x (value): " << x << std::endl;
    };
    x = 200; // 修改外部变量不影响捕获的值
    printX();

    // 引用捕获
    auto printXRef = [&x]()
    {
        std::cout << "Captured x (reference): " << x << std::endl;
    };
    printXRef();

    // 混合捕获
    int y = 50;
    auto mixedCapture = [x, &y]()
    {
        std::cout << "x (value): " << x << ", y (reference): " << y << std::endl;
    };
    y = 150;
    mixedCapture();

    // 捕获所有外部变量（值）
    auto captureAllValue = [=]()
    {
        std::cout << "Capture all (value): x=" << x << ", y=" << y << std::endl;
    };
    captureAllValue();

    // 捕获所有外部变量（引用）
    auto captureAllRef = [&]()
    {
        std::cout << "Capture all (reference): x=" << x << ", y=" << y << std::endl;
    };
    captureAllRef();

    // 可变lambda（可以修改值捕获的变量）
    auto increment = [x]() mutable
    {
        x++; // 可以修改值捕获的变量
        std::cout << "Mutable lambda: x after increment=" << x << std::endl;
    };
    increment();
    std::cout << "Original x after mutable lambda: " << x << std::endl; // 原变量不变

    // Lambda作为函数参数
    std::vector<int> numbers = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10};

    // 使用lambda作为排序函数
    std::sort(numbers.begin(), numbers.end(), [](int a, int b)
              {
                  return a > b; // 降序排序
              });

    std::cout << "Sorted numbers (descending): ";
    for (const auto &num : numbers)
    {
        std::cout << num << " ";
    }
    std::cout << std::endl;

    // Lambda作为回调函数
    auto processNumbers = [](const std::vector<int> &nums,
                             std::function<void(int)> callback)
    {
        for (int num : nums)
        {
            callback(num);
        }
    };

    std::cout << "Even numbers: ";
    processNumbers(numbers, [](int num)
                   {
        if (num % 2 == 0) {
            std::cout << num << " ";
        } });
    std::cout << std::endl;
}

// 4. 右值引用和移动语义
void moveSemantics()
{
    std::cout << "\n4. Move Semantics:" << std::endl;

    // 移动构造函数和移动赋值运算符
    class StringBuffer
    {
    public:
        StringBuffer() : _data(nullptr), _size(0)
        {
            std::cout << "StringBuffer default constructor" << std::endl;
        }

        explicit StringBuffer(size_t size) : _size(size), _data(new char[size])
        {
            std::cout << "StringBuffer constructor with size: " << size << std::endl;
        }

        // 拷贝构造函数
        StringBuffer(const StringBuffer &other) : _size(other._size), _data(new char[other._size])
        {
            std::cout << "StringBuffer copy constructor" << std::endl;
            if (_data && other._data)
            {
                memcpy(_data, other._data, _size);
            }
        }

        // 移动构造函数（C++11）
        StringBuffer(StringBuffer &&other) noexcept : _data(other._data), _size(other._size)
        {
            std::cout << "StringBuffer move constructor" << std::endl;
            // 释放原对象的资源所有权
            other._data = nullptr;
            other._size = 0;
        }

        ~StringBuffer()
        {
            std::cout << "StringBuffer destructor, size: " << _size << std::endl;
            delete[] _data;
        }

        // 拷贝赋值运算符
        StringBuffer &operator=(const StringBuffer &other)
        {
            std::cout << "StringBuffer copy assignment" << std::endl;
            if (this != &other)
            {
                delete[] _data;
                _size = other._size;
                _data = new char[_size];
                if (_data && other._data)
                {
                    memcpy(_data, other._data, _size);
                }
            }
            return *this;
        }

        // 移动赋值运算符（C++11）
        StringBuffer &operator=(StringBuffer &&other) noexcept
        {
            std::cout << "StringBuffer move assignment" << std::endl;
            if (this != &other)
            {
                delete[] _data;
                // 接管资源
                _data = other._data;
                _size = other._size;
                // 释放原对象的资源所有权
                other._data = nullptr;
                other._size = 0;
            }
            return *this;
        }

        size_t size() const
        {
            return _size;
        }

    private:
        char *_data;
        size_t _size;
    };

    // 测试移动语义
    StringBuffer buf1(100);
    StringBuffer buf2(std::move(buf1)); // 调用移动构造函数
    std::cout << "buf1 size after move: " << buf1.size() << std::endl;
    std::cout << "buf2 size after move: " << buf2.size() << std::endl;

    StringBuffer buf3(50);
    buf3 = std::move(buf2); // 调用移动赋值运算符
    std::cout << "buf2 size after move assignment: " << buf2.size() << std::endl;
    std::cout << "buf3 size after move assignment: " << buf3.size() << std::endl;
}

// 5. 智能指针（C++11增强）
void smartPointers()
{
    std::cout << "\n5. Smart Pointers (C++11):" << std::endl;

    // unique_ptr（独占所有权）
    std::unique_ptr<int> up1 = std::make_unique<int>(42);
    std::cout << "unique_ptr value: " << *up1 << std::endl;

    // 所有权转移
    std::unique_ptr<int> up2 = std::move(up1);
    if (!up1)
    {
        std::cout << "up1 is now null after move" << std::endl;
    }
    std::cout << "up2 value: " << *up2 << std::endl;

    // shared_ptr（共享所有权）
    std::shared_ptr<std::string> sp1 = std::make_shared<std::string>("shared");
    std::cout << "shared_ptr value: " << *sp1 << ", use_count: " << sp1.use_count() << std::endl;

    // 共享所有权
    std::shared_ptr<std::string> sp2 = sp1;
    std::cout << "sp1 use_count after copy: " << sp1.use_count() << std::endl;
    std::cout << "sp2 use_count: " << sp2.use_count() << std::endl;

    // weak_ptr（不增加引用计数）
    std::weak_ptr<std::string> wp = sp1;
    std::cout << "weak_ptr use_count: " << wp.use_count() << std::endl;

    // 检查weak_ptr是否有效
    if (auto lock = wp.lock())
    {
        std::cout << "weak_ptr locked: " << *lock << std::endl;
    }
}

class App
{
public:
    App(const std::string &name) : _name(name)
    {
        std::cout << "App " << _name << " created" << std::endl;
    }

    ~App()
    {
        std::cout << "App " << _name << " destroyed" << std::endl;
    }

    void run()
    {
        std::cout << "App " << _name << " running" << std::endl;
    }

    std::string getName() const
    {
        return _name;
    }

private:
    std::string _name;
};

int main()
{
    std::cout << "=== C++11 Features Examples ===" << std::endl;

    autoTypeDeduction();
    rangeBasedForLoop();
    lambdaExpressions();
    moveSemantics();
    smartPointers();

    // 使用C++11特性管理App实例
    std::vector<std::unique_ptr<App>> apps;

    // 使用make_unique创建对象
    apps.push_back(std::make_unique<App>("App1"));
    apps.push_back(std::make_unique<App>("App2"));
    apps.push_back(std::make_unique<App>("App3"));

    // 使用范围for循环遍历智能指针向量
    std::cout << "\nRunning all apps:" << std::endl;
    for (const auto &app : apps)
    {
        app->run();
    }

    // 使用lambda表达式排序
    std::sort(apps.begin(), apps.end(), [](const std::unique_ptr<App> &a, const std::unique_ptr<App> &b)
              { return a->getName() < b->getName(); });

    std::cout << "\nApps after sorting:" << std::endl;
    for (const auto &app : apps)
    {
        std::cout << "  " << app->getName() << std::endl;
    }

    std::cout << "\n=== C++11 Features Examples Finished ===" << std::endl;

    return 0;
}