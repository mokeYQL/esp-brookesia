/*
 * 知识点：C++17特性（结构化绑定、if constexpr、折叠表达式）
 * 源项目：e:\3.esp-brookesia\esp-brookesia\core\brookesia_core\systems\base\esp_brookesia_base_app.hpp 第28-76行
 * C++ Primer中文版（第五版）：第10章 泛型算法 - 10.3 定制操作
 * Effective+C++中文版（第三版）：无直接对应章节
 */

#include <iostream>
#include <string>
#include <vector>
#include <map>
#include <tuple>
#include <optional>
#include <variant>
#include <any>
#include <memory>
#include <algorithm>

// 1. 结构化绑定（C++17）
void useStructuredBindings()
{
    std::cout << "\n1. Structured Bindings (C++17):" << std::endl;

    // 绑定到pair
    std::map<std::string, int> map = {{"apple", 100}, {"banana", 200}, {"cherry", 300}};

    for (const auto &[key, value] : map)
    {
        std::cout << "Key: " << key << ", Value: " << value << std::endl;
    }

    // 绑定到tuple
    auto tupleVal = std::make_tuple(10, 3.14, "hello");
    auto [i, d, s] = tupleVal;
    std::cout << "Tuple values: " << i << ", " << d << ", " << s << std::endl;

    // 绑定到结构体
    struct Point
    {
        int x;
        int y;
        std::string name;
    };

    Point p = {10, 20, "Point1"};
    auto [x, y, name] = p;
    std::cout << "Point: " << name << ", x=" << x << ", y=" << y << std::endl;

    // 绑定到数组
    int arr[] = {1, 2, 3, 4, 5};
    auto [a, b, c, d2, e] = arr;
    std::cout << "Array values: " << a << ", " << b << ", " << c << ", " << d2 << ", " << e << std::endl;

    // 绑定到引用
    Point &refP = p;
    auto &[refX, refY, refName] = refP;
    refX = 100; // 修改原结构体
    std::cout << "Point after modification: " << p.name << ", x=" << p.x << ", y=" << p.y << std::endl;
}

// 2. if constexpr（C++17）
template <typename T>
void useIfConstexpr(const T &value)
{
    std::cout << "\n2. if constexpr (C++17):" << std::endl;

    // if constexpr 编译期条件判断
    if constexpr (std::is_integral<T>::value)
    {
        std::cout << "Value is integral: " << value << std::endl;
    }
    else if constexpr (std::is_floating_point<T>::value)
    {
        std::cout << "Value is floating point: " << value << std::endl;
    }
    else if constexpr (std::is_same<T, std::string>::value)
    {
        std::cout << "Value is string: " << value << std::endl;
    }
    else
    {
        std::cout << "Value is of unknown type: " << typeid(value).name() << std::endl;
    }
}

// 3. 折叠表达式（C++17）
// 一元左折叠
template <typename... Args>
auto sumLeft(Args... args)
{
    return (... + args); // 展开为 ((a + b) + c) + d
}

// 一元右折叠
template <typename... Args>
auto sumRight(Args... args)
{
    return (args + ...); // 展开为 a + (b + (c + d))
}

// 二元折叠
template <typename... Args>
auto sumWithInit(Args... args)
{
    return (0 + ... + args); // 展开为 (((0 + a) + b) + c) + d
}

// 折叠表达式与lambda
template <typename... Args>
void printAll(Args... args)
{
    (std::cout << ... << args) << std::endl;
}

// 先声明process函数
template <typename T>
void process(T value);

// 具体process函数实现
void process(int value)
{
    std::cout << "Processing int: " << value << std::endl;
}

void process(double value)
{
    std::cout << "Processing double: " << value << std::endl;
}

void process(const std::string &value)
{
    std::cout << "Processing string: " << value << std::endl;
}

// 折叠表达式与逗号
template <typename... Args>
void processAll(Args... args)
{
    (process(args), ...);
}

void useFoldExpressions()
{
    std::cout << "\n3. Fold Expressions (C++17):" << std::endl;

    auto sum1 = sumLeft(1, 2, 3, 4, 5);
    auto sum2 = sumRight(1, 2, 3, 4, 5);
    auto sum3 = sumWithInit(1, 2, 3, 4, 5);

    std::cout << "sumLeft(1, 2, 3, 4, 5): " << sum1 << std::endl;
    std::cout << "sumRight(1, 2, 3, 4, 5): " << sum2 << std::endl;
    std::cout << "sumWithInit(1, 2, 3, 4, 5): " << sum3 << std::endl;

    // 使用折叠表达式打印
    printAll("Hello, ", "world! ", 42, " ", 3.14);

    // 使用折叠表达式处理所有参数
    processAll(10, 3.14, std::string("test"), 20, 5.5);
}

// 4. std::optional（C++17）
std::optional<int> findInVector(const std::vector<int> &vec, int value)
{
    auto it = std::find(vec.begin(), vec.end(), value);
    if (it != vec.end())
    {
        return *it;
    }
    return std::nullopt; // 表示无值
}

void useOptional()
{
    std::cout << "\n4. std::optional (C++17):" << std::endl;

    std::vector<int> vec = {1, 2, 3, 4, 5};

    // 找到值
    auto result1 = findInVector(vec, 3);
    if (result1.has_value())
    {
        std::cout << "Found value: " << result1.value() << std::endl;
    }

    // 使用operator*
    if (result1)
    {
        std::cout << "Found value using operator*: " << *result1 << std::endl;
    }

    // 使用value_or获取默认值
    auto result2 = findInVector(vec, 10);
    std::cout << "Value or default: " << result2.value_or(0) << std::endl;

    // 修改optional值
    auto opt = std::optional<int>(42);
    opt = 100;
    std::cout << "Modified optional: " << opt.value() << std::endl;
}

// 5. std::variant（C++17）
void useVariant()
{
    std::cout << "\n5. std::variant (C++17):" << std::endl;

    // 基本使用
    std::variant<int, double, std::string> var;

    var = 10; // 存储int
    std::cout << "Variant holds int: " << std::get<int>(var) << std::endl;

    var = 3.14; // 存储double
    std::cout << "Variant holds double: " << std::get<double>(var) << std::endl;

    var = "hello"; // 存储string
    std::cout << "Variant holds string: " << std::get<std::string>(var) << std::endl;

    // 使用index获取当前类型索引
    std::cout << "Current variant index: " << var.index() << std::endl;

    // 使用std::holds_alternative检查类型
    if (std::holds_alternative<std::string>(var))
    {
        std::cout << "Variant currently holds a string" << std::endl;
    }

    // 使用std::visit访问variant
    auto visitor = [](auto &&arg)
    {
        using T = std::decay_t<decltype(arg)>;
        if constexpr (std::is_same_v<T, int>)
        {
            std::cout << "Visitor int: " << arg << std::endl;
        }
        else if constexpr (std::is_same_v<T, double>)
        {
            std::cout << "Visitor double: " << arg << std::endl;
        }
        else if constexpr (std::is_same_v<T, std::string>)
        {
            std::cout << "Visitor string: " << arg << std::endl;
        }
    };

    std::visit(visitor, var);

    var = 42;
    std::visit(visitor, var);

    var = 2.71;
    std::visit(visitor, var);
}

// 6. std::any（C++17）
void useAny()
{
    std::cout << "\n6. std::any (C++17):" << std::endl;

    // 基本使用
    std::any any_var;

    any_var = 10;
    std::cout << "Any holds int: " << std::any_cast<int>(any_var) << std::endl;

    any_var = 3.14;
    std::cout << "Any holds double: " << std::any_cast<double>(any_var) << std::endl;

    any_var = std::string("hello");
    std::cout << "Any holds string: " << std::any_cast<std::string>(any_var) << std::endl;

    // 检查类型
    std::cout << "Any has value: " << any_var.has_value() << std::endl;
    std::cout << "Any type name: " << any_var.type().name() << std::endl;

    // 安全转换
    try
    {
        std::cout << "Trying to cast to int: " << std::any_cast<int>(any_var) << std::endl;
    }
    catch (const std::bad_any_cast &e)
    {
        std::cout << "Bad any cast: " << e.what() << std::endl;
    }

    // 重置any
    any_var.reset();
    std::cout << "Any has value after reset: " << any_var.has_value() << std::endl;
}

// 7. constexpr if 增强（C++17）
template <typename T>
auto getValue(T &&value)
{
    if constexpr (std::is_integral_v<std::decay_t<T>>)
    {
        return value * 2;
    }
    else if constexpr (std::is_floating_point_v<std::decay_t<T>>)
    {
        return value * 3.0;
    }
    else if constexpr (std::is_convertible_v<std::decay_t<T>, std::string>)
    {
        return std::string("String: ") + std::forward<T>(value);
    }
    else
    {
        return value;
    }
}

void useConstexprIfEnhanced()
{
    std::cout << "\n7. Enhanced constexpr if (C++17):" << std::endl;

    auto result1 = getValue(10);
    auto result2 = getValue(3.14);
    auto result3 = getValue(std::string("hello"));
    auto result4 = getValue("world");

    std::cout << "getValue(10): " << result1 << ", type: " << typeid(result1).name() << std::endl;
    std::cout << "getValue(3.14): " << result2 << ", type: " << typeid(result2).name() << std::endl;
    std::cout << "getValue(string(\"hello\")): " << result3 << ", type: " << typeid(result3).name() << std::endl;
    std::cout << "getValue(\"world\"): " << result4 << ", type: " << typeid(result4).name() << std::endl;
}

// 8. 内联变量（C++17）
inline constexpr double PI = 3.14159265358979323846;
inline const std::string APP_NAME = "ESP-Brookesia";
inline int GLOBAL_COUNTER = 0;

void useInlineVariables()
{
    std::cout << "\n8. Inline Variables (C++17):" << std::endl;

    std::cout << "PI: " << PI << std::endl;
    std::cout << "APP_NAME: " << APP_NAME << std::endl;
    std::cout << "GLOBAL_COUNTER: " << GLOBAL_COUNTER << std::endl;

    // 修改内联变量
    GLOBAL_COUNTER++;
    std::cout << "GLOBAL_COUNTER after increment: " << GLOBAL_COUNTER << std::endl;

    // 在不同函数中使用
    [&]()
    {
        GLOBAL_COUNTER++;
        std::cout << "GLOBAL_COUNTER in lambda: " << GLOBAL_COUNTER << std::endl;
    }();
}

// 9. 模板参数推导（C++17）
template <typename T>
class Box
{
public:
    Box(T value) : _value(value) {}

    T getValue() const
    {
        return _value;
    }

private:
    T _value;
};

// 推导指南（C++17）
template <typename T>
Box(T) -> Box<T>;

void useTemplateDeduction()
{
    std::cout << "\n9. Template Argument Deduction (C++17):" << std::endl;

    // 自动推导模板参数
    Box box1(10);                   // 推导为 Box<int>
    Box box2(3.14);                 // 推导为 Box<double>
    Box box3(std::string("hello")); // 推导为 Box<std::string>

    std::cout << "box1 value: " << box1.getValue() << ", type: " << typeid(box1.getValue()).name() << std::endl;
    std::cout << "box2 value: " << box2.getValue() << ", type: " << typeid(box2.getValue()).name() << std::endl;
    std::cout << "box3 value: " << box3.getValue() << ", type: " << typeid(box3.getValue()).name() << std::endl;

    // 使用make_unique和make_shared的类似模式
    auto createBox = [](auto value)
    {
        return Box(value);
    };

    auto box4 = createBox(20);
    std::cout << "box4 value: " << box4.getValue() << std::endl;
}

// 10. __has_include 预处理器指令（C++17）
void useHasInclude()
{
    std::cout << "\n10. __has_include Preprocessor (C++17):" << std::endl;

    // 检查头文件是否存在
#if __has_include(<vector>)
    std::cout << "<vector> is available" << std::endl;
#else
    std::cout << "<vector> is not available" << std::endl;
#endif

#if __has_include(<nonexistent_header.h>)
    std::cout << "<nonexistent_header.h> is available" << std::endl;
#else
    std::cout << "<nonexistent_header.h> is not available" << std::endl;
#endif

    // 可以结合条件编译
#if __has_include(<optional>)
    std::cout << "Using <optional> header" << std::endl;
#else
    std::cout << "<optional> not available, using alternative" << std::endl;
#endif
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
    std::cout << "=== C++17 Features Examples ===" << std::endl;

    useStructuredBindings();
    useConstexprIfEnhanced();
    useFoldExpressions();
    useOptional();
    useVariant();
    useAny();
    useConstexprIfEnhanced();
    useInlineVariables();
    useTemplateDeduction();
    useHasInclude();

    // 使用C++17特性管理App实例
    std::vector<std::unique_ptr<App>> apps;

    // 使用emplace_back避免拷贝
    apps.emplace_back(std::make_unique<App>("App1"));
    apps.emplace_back(std::make_unique<App>("App2"));
    apps.emplace_back(std::make_unique<App>("App3"));

    // 使用结构化绑定遍历
    for (size_t i = 0; i < apps.size(); ++i)
    {
        auto &app = apps[i];
        std::cout << "App " << i << ": " << app->getName() << std::endl;
        app->run();
    }

    std::cout << "\n=== C++17 Features Examples Finished ===" << std::endl;

    return 0;
}