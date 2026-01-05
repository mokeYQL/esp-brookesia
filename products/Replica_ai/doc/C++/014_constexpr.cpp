/*
 * 知识点：常量表达式
 * 源项目：e:\3.esp-brookesia\esp-brookesia\core\brookesia_core\systems\base\esp_brookesia_base_app.hpp 第40-52行
 * C++ Primer中文版（第五版）：第2章 变量和基本类型 - 2.4 const限定符
 * Effective+C++中文版（第三版）：条款2 尽量以const、enum、inline替换#define
 */

#include <iostream>
#include <string>
#include <array>
#include <cmath>

// 1. 常量表达式变量
constexpr int SCREEN_WIDTH = 320;
constexpr int SCREEN_HEIGHT = 240;
constexpr double PI = 3.14159265358979323846;
constexpr int MAX_APPS = 10;

// 2. 常量表达式函数
constexpr int square(int x)
{
    return x * x;
}

constexpr double circleArea(double radius)
{
    return PI * radius * radius;
}

constexpr int max(int a, int b)
{
    return (a > b) ? a : b;
}

constexpr bool isEven(int n)
{
    return (n % 2 == 0);
}

// 3. 常量表达式结构体
struct AppConfig
{
    const char *name;
    int id;
    bool enabled;

    // 常量表达式构造函数
    constexpr AppConfig(const char *app_name, int app_id, bool app_enabled)
        : name(app_name), id(app_id), enabled(app_enabled) {}
};

// 4. 常量表达式数组
constexpr std::array<int, 5> createArray()
{
    std::array<int, 5> arr = {0};
    for (int i = 0; i < 5; ++i)
    {
        arr[i] = i * 2;
    }
    return arr;
}

constexpr std::array<AppConfig, 3> APP_CONFIGS = {
    AppConfig("Calculator", 1, true),
    AppConfig("Game", 2, true),
    AppConfig("Settings", 3, false)};

// 5. 模板与常量表达式 - 使用迭代实现避免递归深度问题
constexpr int factorial(int n)
{
    int result = 1;
    for (int i = 1; i <= n; ++i)
    {
        result *= i;
    }
    return result;
}

// 6. 编译期计算示例
constexpr int SCREEN_AREA = SCREEN_WIDTH * SCREEN_HEIGHT;
constexpr int MAX_SQUARE = square(MAX_APPS);
constexpr double CIRCLE_AREA = circleArea(5.0);
constexpr int MAX_VALUE = max(10, 20);
constexpr bool IS_EVEN = isEven(10);
constexpr int FACTORIAL_5 = factorial(5);

class Display
{
public:
    // 使用常量表达式作为模板参数
    static constexpr int DEFAULT_WIDTH = SCREEN_WIDTH;
    static constexpr int DEFAULT_HEIGHT = SCREEN_HEIGHT;

    // 常量表达式构造函数
    constexpr Display(int width = DEFAULT_WIDTH, int height = DEFAULT_HEIGHT)
        : _width(width), _height(height), _area(width * height)
    {
        // 构造函数体可以包含简单的语句（C++14及以上）
        if (_width <= 0 || _height <= 0)
        {
            _width = DEFAULT_WIDTH;
            _height = DEFAULT_HEIGHT;
            _area = _width * _height;
        }
    }

    // 常量表达式成员函数
    constexpr int getWidth() const
    {
        return _width;
    }

    constexpr int getHeight() const
    {
        return _height;
    }

    constexpr int getArea() const
    {
        return _area;
    }

    // 常量表达式修改函数（C++17及以上）
    constexpr void resize(int newWidth, int newHeight)
    {
        if (newWidth > 0 && newHeight > 0)
        {
            _width = newWidth;
            _height = newHeight;
            _area = _width * _height;
        }
    }

private:
    int _width;
    int _height;
    int _area;
};

// 7. 编译期验证
static_assert(SCREEN_WIDTH > 0, "Screen width must be positive");
static_assert(SCREEN_HEIGHT > 0, "Screen height must be positive");
static_assert(MAX_APPS > 0, "Max apps must be positive");
static_assert(PI > 3.0, "PI must be greater than 3.0");
static_assert(square(5) == 25, "Square of 5 must be 25");
static_assert(factorial(5) == 120, "Factorial of 5 must be 120");

int main()
{
    std::cout << "=== constexpr Examples ===" << std::endl;

    std::cout << "\n1. constexpr Variables:" << std::endl;
    std::cout << "SCREEN_WIDTH: " << SCREEN_WIDTH << std::endl;
    std::cout << "SCREEN_HEIGHT: " << SCREEN_HEIGHT << std::endl;
    std::cout << "PI: " << PI << std::endl;
    std::cout << "MAX_APPS: " << MAX_APPS << std::endl;

    std::cout << "\n2. constexpr Functions (Compile-time Calculations):" << std::endl;
    std::cout << "SCREEN_AREA: " << SCREEN_AREA << std::endl;
    std::cout << "MAX_SQUARE: " << MAX_SQUARE << std::endl;
    std::cout << "CIRCLE_AREA: " << CIRCLE_AREA << std::endl;
    std::cout << "MAX_VALUE: " << MAX_VALUE << std::endl;
    std::cout << "IS_EVEN: " << (IS_EVEN ? "true" : "false") << std::endl;
    std::cout << "FACTORIAL_5: " << FACTORIAL_5 << std::endl;

    std::cout << "\n3. constexpr Structs:" << std::endl;
    for (const auto &config : APP_CONFIGS)
    {
        std::cout << "App: " << config.name << ", ID: " << config.id << ", Enabled: " << (config.enabled ? "true" : "false") << std::endl;
    }

    std::cout << "\n4. constexpr Objects:" << std::endl;
    // 编译期创建对象
    constexpr Display display1;
    constexpr Display display2(480, 320);

    std::cout << "Display1: " << display1.getWidth() << "x" << display1.getHeight() << ", Area: " << display1.getArea() << std::endl;
    std::cout << "Display2: " << display2.getWidth() << "x" << display2.getHeight() << ", Area: " << display2.getArea() << std::endl;

    // 运行期创建对象
    Display display3(800, 600);
    std::cout << "Display3 (runtime): " << display3.getWidth() << "x" << display3.getHeight() << ", Area: " << display3.getArea() << std::endl;

    // 运行期修改对象
    display3.resize(1024, 768);
    std::cout << "Display3 (resized): " << display3.getWidth() << "x" << display3.getHeight() << ", Area: " << display3.getArea() << std::endl;

    std::cout << "\n5. constexpr Array:" << std::endl;
    constexpr auto arr = createArray();
    for (size_t i = 0; i < arr.size(); ++i)
    {
        std::cout << "arr[" << i << "] = " << arr[i] << std::endl;
    }

    std::cout << "\n6. constexpr Function:" << std::endl;
    std::cout << "Factorial of 5: " << factorial(5) << std::endl;
    std::cout << "Factorial of 10: " << factorial(10) << std::endl;

    return 0;
}