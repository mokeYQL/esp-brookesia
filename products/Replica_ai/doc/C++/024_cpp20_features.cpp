/*
 * 知识点：C++17特性（替代C++20特性）
 * 源项目：e:\3.esp-brookesia\esp-brookesia\products\speaker\main\modules\system.cpp 第31-36行
 * C++ Primer中文版（第五版）：第12章 动态内存 - 12.1 动态内存与智能指针
 * Effective+C++中文版（第三版）：条款2 尽量以const、enum、inline替换#define
 */
#include <iostream>
#include <string>
#include <vector>
#include <string_view>
#include <algorithm>

// 1. 演示constexpr函数
constexpr int calculate_max_wait_count(int max_ms, int interval_ms)
{
    return max_ms / interval_ms;
}

// 2. 演示constexpr变量
constexpr int GLOBAL_THREAD_NAME_MAX_LENGTH = 20;
constexpr int GLOBAL_DEFAULT_VOLUME = 50;
constexpr int HARDWARE_TIMER_FREQUENCY = 1000; // 1kHz
constexpr int MAX_DELAY_MS = 5000;

// 3. 演示constexpr结构体初始化
struct ThreadConfig
{
    const char *name;
    size_t stack_size;
    bool stack_in_ext;
};

// 编译期配置线程
constexpr ThreadConfig create_thread_config(const char *name, size_t stack_size, bool stack_in_ext)
{
    return {
        name,
        stack_size,
        stack_in_ext};
}

// 4. 演示constexpr函数用于硬件配置
constexpr int get_max_timer_ticks()
{
    return MAX_DELAY_MS * HARDWARE_TIMER_FREQUENCY / 1000;
}

// 5. 演示简化版三路比较（使用传统比较运算符）
struct Point
{
    int x;
    int y;

    // C++17: 传统比较运算符
    bool operator==(const Point &other) const
    {
        return x == other.x && y == other.y;
    }

    bool operator!=(const Point &other) const
    {
        return !(*this == other);
    }

    bool operator<(const Point &other) const
    {
        if (x != other.x)
            return x < other.x;
        return y < other.y;
    }

    bool operator<=(const Point &other) const
    {
        return *this < other || *this == other;
    }

    bool operator>(const Point &other) const
    {
        return !(*this <= other);
    }

    bool operator>=(const Point &other) const
    {
        return !(*this < other);
    }
};

// 6. 演示std::string_view（C++17特性）
void demonstrate_string_view()
{
    std::cout << "\n=== Demonstrating std::string_view (C++17) ===" << std::endl;

    std::string full_string = "Hello, C++17!";
    std::string_view sv(full_string);

    std::cout << "Original string: " << full_string << std::endl;
    std::cout << "string_view: " << sv << std::endl;
    std::cout << "string_view length: " << sv.length() << std::endl;

    // 子字符串
    std::string_view sv_sub = sv.substr(7, 5);
    std::cout << "string_view substring: " << sv_sub << std::endl;
}

// 7. 演示if constexpr（C++17特性）
template <typename T>
void process_value(T value)
{
    if constexpr (std::is_integral_v<T>)
    {
        std::cout << "Processing integral value: " << value << std::endl;
    }
    else if constexpr (std::is_floating_point_v<T>)
    {
        std::cout << "Processing floating point value: " << value << std::endl;
    }
    else
    {
        std::cout << "Processing other type" << std::endl;
    }
}

// 8. 演示结构化绑定（C++17特性）
void demonstrate_structured_bindings()
{
    std::cout << "\n=== Demonstrating structured bindings (C++17) ===" << std::endl;

    Point p = {10, 20};
    auto [x, y] = p;
    std::cout << "Point coordinates: x = " << x << ", y = " << y << std::endl;

    // 修改绑定的变量
    x = 30;
    std::cout << "After modifying x in binding: " << std::endl;
    std::cout << "Binding x: " << x << std::endl;
    std::cout << "Original point x: " << p.x << std::endl; // 注意：结构化绑定默认是值绑定

    // 引用绑定
    auto &[rx, ry] = p;
    rx = 40;
    std::cout << "After modifying rx in reference binding: " << std::endl;
    std::cout << "Binding rx: " << rx << std::endl;
    std::cout << "Original point x: " << p.x << std::endl; // 现在修改了原始对象
}

// 9. 演示折叠表达式（C++17特性）
template <typename... Args>
auto sum(Args... args)
{
    return (args + ...);
}

// 10. 演示constexpr if和折叠表达式结合
template <typename... Args>
void print_all(Args... args)
{
    (std::cout << ... << args) << std::endl;
}

// 11. 演示inline变量（C++17特性）
inline std::string_view GLOBAL_THREAD_PREFIX = "thread_";

// 12. 演示std::optional的简化替代方案（C++17版本）
template <typename T>
class Optional
{
public:
    Optional() : has_value_(false) {}
    Optional(const T &value) : has_value_(true), value_(value) {}

    bool has_value() const { return has_value_; }
    const T &value() const { return value_; }

private:
    bool has_value_;
    T value_;
};

// 13. 演示简化版consteval/constinit（使用constexpr替代）
constexpr Point create_point(int x, int y)
{
    return {x, y};
}

// 主函数
int main()
{
    std::cout << "=== Testing C++17 Features ===" << std::endl;

    // 使用constexpr函数在编译期计算
    constexpr int max_wait_count = calculate_max_wait_count(2000, 10);
    std::cout << "Max wait count (constexpr): " << max_wait_count << std::endl;

    // 使用constexpr变量
    std::cout << "Global default volume (constexpr): " << GLOBAL_DEFAULT_VOLUME << std::endl;

    // 演示constexpr函数用于硬件配置
    constexpr int max_timer_ticks = get_max_timer_ticks();
    std::cout << "Max timer ticks (constexpr): " << max_timer_ticks << std::endl;

    // 演示编译期线程配置
    constexpr ThreadConfig app_thread_config = create_thread_config(
        "open_app",
        20 * 1024,
        true);

    std::cout << "\nCompiled thread config:" << std::endl;
    std::cout << "  Name: " << app_thread_config.name << std::endl;
    std::cout << "  Stack size: " << app_thread_config.stack_size << " bytes" << std::endl;
    std::cout << "  Stack in ext: " << (app_thread_config.stack_in_ext ? "true" : "false") << std::endl;

    // 演示string_view
    demonstrate_string_view();

    // 演示constexpr if
    std::cout << "\n=== Demonstrating if constexpr (C++17) ===" << std::endl;
    process_value(42);
    process_value(3.14);
    process_value("hello");

    // 演示结构化绑定
    demonstrate_structured_bindings();

    // 演示折叠表达式
    std::cout << "\n=== Demonstrating fold expressions (C++17) ===" << std::endl;
    int sum_result = sum(1, 2, 3, 4, 5);
    std::cout << "Sum of 1+2+3+4+5: " << sum_result << std::endl;

    // 演示print_all with fold expressions
    std::cout << "Print with fold expressions: ";
    print_all("Hello", ", ", "C++17", "!\n");

    // 演示简化版optional
    Optional<int> opt1;
    Optional<int> opt2(42);
    std::cout << "\n=== Demonstrating simplified optional (C++17) ===" << std::endl;
    std::cout << "opt1.has_value(): " << (opt1.has_value() ? "true" : "false") << std::endl;
    std::cout << "opt2.has_value(): " << (opt2.has_value() ? "true" : "false") << std::endl;
    if (opt2.has_value())
    {
        std::cout << "opt2.value(): " << opt2.value() << std::endl;
    }

    // 演示constexpr点创建
    constexpr Point p = create_point(100, 200);
    std::cout << "\n=== Demonstrating constexpr point creation ===" << std::endl;
    std::cout << "Point coordinates: x=" << p.x << ", y=" << p.y << std::endl;

    std::cout << "\nAll C++17 features demonstrated successfully!" << std::endl;

    return 0;
}
