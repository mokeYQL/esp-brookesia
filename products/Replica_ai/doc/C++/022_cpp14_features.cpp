/*
 * 知识点：C++14特性（auto返回类型推导、泛型lambda、变量模板）
 * 源项目：e:\3.esp-brookesia\esp-brookesia\core\brookesia_core\systems\base\esp_brookesia_base_app.hpp 第40-52行
 * C++ Primer中文版（第五版）：第12章 动态内存 - 12.1.2 shared_ptr类
 * Effective+C++中文版（第三版）：无直接对应章节
 */

#include <iostream>
#include <string>
#include <vector>
#include <map>
#include <algorithm>
#include <functional>

// 1. 函数返回类型推导（C++14）
auto add(int a, int b)
{
    return a + b;
}

auto multiply(double a, double b)
{
    return a * b;
}

auto concatenate(const std::string &a, const std::string &b)
{
    return a + b;
}

// 复杂返回类型推导 - 移到外部
auto getVector()
{
    return std::vector<int>{1, 2, 3, 4, 5};
}

// 返回类型为引用 - 移到外部
int global = 100;
auto &getGlobalRef()
{
    return global;
}

void useAutoReturnType()
{
    std::cout << "\n1. Auto Return Type Deduction (C++14):" << std::endl;

    auto result1 = add(10, 20);
    auto result2 = multiply(3.14, 2.0);
    auto result3 = concatenate("Hello, ", "world!");

    std::cout << "add(10, 20) = " << result1 << ", type: " << typeid(result1).name() << std::endl;
    std::cout << "multiply(3.14, 2.0) = " << result2 << ", type: " << typeid(result2).name() << std::endl;
    std::cout << "concatenate(\"Hello, \", \"world!\") = " << result3 << ", type: " << typeid(result3).name() << std::endl;

    // 使用外部函数
    auto vec = getVector();
    std::cout << "getVector() size: " << vec.size() << std::endl;
    for (const auto &elem : vec)
    {
        std::cout << elem << " ";
    }
    std::cout << std::endl;

    // 使用外部函数
    auto &ref = getGlobalRef();
    ref = 200;
    std::cout << "global after modification: " << global << std::endl;
}

// 2. 泛型Lambda表达式（C++14）
void useGenericLambda()
{
    std::cout << "\n2. Generic Lambda (C++14):" << std::endl;

    // 泛型lambda：使用auto作为参数类型
    auto genericAdd = [](auto a, auto b)
    {
        return a + b;
    };

    int result1 = genericAdd(10, 20);                                              // int + int
    double result2 = genericAdd(3.14, 2.71);                                       // double + double
    std::string result3 = genericAdd(std::string("apple"), std::string("banana")); // string + string

    std::cout << "genericAdd(10, 20) = " << result1 << std::endl;
    std::cout << "genericAdd(3.14, 2.71) = " << result2 << std::endl;
    std::cout << "genericAdd(\"apple\", \"banana\") = " << result3 << std::endl;

    // 泛型lambda与STL算法结合
    std::vector<int> vec = {1, 2, 3, 4, 5};
    std::cout << "Original vector: ";
    for (const auto &elem : vec)
        std::cout << elem << " ";
    std::cout << std::endl;

    // 使用泛型lambda作为transform的第三个参数
    std::transform(vec.begin(), vec.end(), vec.begin(), [](auto x)
                   { return x * 2; });
    std::cout << "Vector after doubling: ";
    for (const auto &elem : vec)
        std::cout << elem << " ";
    std::cout << std::endl;

    // 泛型lambda捕获外部变量
    int factor = 3;
    auto multiplyBy = [factor](auto x)
    { return x * factor; };

    std::vector<double> dvec = {1.1, 2.2, 3.3};
    std::cout << "Original double vector: ";
    for (const auto &elem : dvec)
        std::cout << elem << " ";
    std::cout << std::endl;

    std::transform(dvec.begin(), dvec.end(), dvec.begin(), multiplyBy);
    std::cout << "Double vector after multiplying by " << factor << ": ";
    for (const auto &elem : dvec)
        std::cout << elem << " ";
    std::cout << std::endl;
}

// 3. 变量模板（C++14）
template <typename T>
constexpr T pi = T(3.14159265358979323846);

template <typename T>
constexpr T e = T(2.71828182845904523536);

// 类模板的变量模板
template <typename T>
class Vector
{
public:
    // 静态变量模板
    template <typename U>
    static constexpr U default_value = U{};
};

void useVariableTemplates()
{
    std::cout << "\n3. Variable Templates (C++14):" << std::endl;

    // 使用变量模板
    std::cout << "pi<int>: " << pi<int> << std::endl;
    std::cout << "pi<double>: " << pi<double> << std::endl;
    std::cout << "pi<float>: " << pi<float> << std::endl;

    std::cout << "e<int>: " << e<int> << std::endl;
    std::cout << "e<double>: " << e<double> << std::endl;

    // 计算圆面积
    auto circleArea = [](auto radius)
    {
        return pi<decltype(radius)> * radius * radius;
    };

    std::cout << "Circle area (radius=5): " << circleArea(5) << std::endl;
    std::cout << "Circle area (radius=5.5): " << circleArea(5.5) << std::endl;

    // 使用类模板的变量模板
    std::cout << "Vector<int>::default_value<int>: " << Vector<int>::default_value<int> << std::endl;
    std::cout << "Vector<int>::default_value<double>: " << Vector<int>::default_value<double> << std::endl;
    // 移除string类型，因为它不是字面量类型，不能用于constexpr变量
    // std::cout << "Vector<double>::default_value<std::string>: " << Vector<double>::default_value<std::string>() << std::endl;
}

// 4. Lambda捕获初始化（C++14）
void useLambdaInitCapture()
{
    std::cout << "\n4. Lambda Init Capture (C++14):" << std::endl;

    // 传统方式：先创建变量，然后捕获
    int x = 10;
    auto lambda1 = [x]()
    {
        return x * 2;
    };
    std::cout << "Traditional capture: " << lambda1() << std::endl;

    // C++14：初始化捕获（也称为广义lambda捕获）
    auto lambda2 = [x = 20]()
    {
        return x * 2;
    };
    std::cout << "Init capture: " << lambda2() << std::endl;

    // 使用std::move进行移动捕获
    std::string s = "Hello, world!";
    auto lambda3 = [str = std::move(s)]()
    {
        return str + " Captured by move!";
    };

    std::cout << "Lambda with move capture: " << lambda3() << std::endl;
    std::cout << "Original string after move: '" << s << "'" << std::endl;

    // 初始化捕获与泛型lambda结合
    auto createAdder = [factor = 5](auto x)
    {
        return x + factor;
    };

    std::cout << "createAdder(10): " << createAdder(10) << std::endl;
    std::cout << "createAdder(10.5): " << createAdder(10.5) << std::endl;

    // 捕获this指针（传统方式）
    class MyClass
    {
    public:
        MyClass(int value) : _value(value) {}

        void demo()
        {
            // 传统方式：捕获this
            auto lambda4 = [this]()
            {
                return _value * 2;
            };

            // C++14：捕获*this（复制当前对象）
            auto lambda5 = [*this]()
            {
                return _value * 3;
            };

            std::cout << "Lambda with this capture: " << lambda4() << std::endl;
            std::cout << "Lambda with *this capture: " << lambda5() << std::endl;

            // 修改当前对象
            _value = 100;

            std::cout << "After modifying _value to 100: " << std::endl;
            std::cout << "Lambda with this capture: " << lambda4() << std::endl;  // 看到修改后的值
            std::cout << "Lambda with *this capture: " << lambda5() << std::endl; // 看到原始值（复制）
        }

    private:
        int _value;
    };

    MyClass obj(42);
    obj.demo();
}

// 5. constexpr函数增强（C++14）
constexpr int factorial(int n)
{
    // C++14允许constexpr函数有if语句
    if (n <= 1)
    {
        return 1;
    }
    return n * factorial(n - 1);
}

constexpr int fibonacci(int n)
{
    // C++14允许constexpr函数有循环
    if (n <= 1)
    {
        return n;
    }

    int a = 0, b = 1, c = 0; // 初始化c为0
    for (int i = 2; i <= n; i++)
    {
        c = a + b;
        a = b;
        b = c;
    }
    return b;
}

// 使用模板参数代替auto参数，兼容C++17
template <typename T>
constexpr T power(T base, int exponent)
{
    T result = base;
    for (int i = 1; i < exponent; i++)
    {
        result *= base;
    }
    return result;
}

void useEnhancedConstexpr()
{
    std::cout << "\n5. Enhanced constexpr (C++14):" << std::endl;

    // 编译期计算阶乘
    constexpr int fact5 = factorial(5);
    constexpr int fact10 = factorial(10);

    std::cout << "factorial(5): " << fact5 << std::endl;
    std::cout << "factorial(10): " << fact10 << std::endl;

    // 编译期计算斐波那契数
    constexpr int fib10 = fibonacci(10);
    constexpr int fib20 = fibonacci(20);

    std::cout << "fibonacci(10): " << fib10 << std::endl;
    std::cout << "fibonacci(20): " << fib20 << std::endl;

    // 泛型constexpr函数
    constexpr auto pow2_10 = power(2, 10);
    constexpr auto pow3_5 = power(3, 5);
    constexpr auto pow10_3 = power(10, 3);

    std::cout << "power(2, 10): " << pow2_10 << std::endl;
    std::cout << "power(3, 5): " << pow3_5 << std::endl;
    std::cout << "power(10, 3): " << pow10_3 << std::endl;

    // 运行时也可以调用constexpr函数
    int n;
    std::cout << "Enter a number to compute factorial: ";
    std::cin >> n;
    std::cout << "factorial(" << n << ") = " << factorial(n) << std::endl;
}

// 6. decltype(auto) 用于变量和返回类型（C++14）
// 移到外部的函数 - 静态变量版本
static auto &getRef()
{
    static int value = 42;
    return value;
}

// 全局变量用于演示
auto g_x = 10;
auto &g_ref = g_x;

// 移到外部的函数 - 使用全局变量
decltype(auto) getValue()
{
    return g_x; // 返回int
}

decltype(auto) getRefValue()
{
    return g_ref; // 返回int&
}

void useDecltypeAuto()
{
    std::cout << "\n6. decltype(auto) (C++14):" << std::endl;

    int x = 10;
    int &ref = x;
    const int &const_ref = x;

    // 用于变量
    decltype(auto) var1 = x;         // int
    decltype(auto) var2 = ref;       // int& （保留引用）
    decltype(auto) var3 = const_ref; // const int& （保留const引用）
    decltype(auto) var4 = (x);       // int& （括号导致返回引用）

    std::cout << "var1 type: " << typeid(var1).name() << std::endl;
    std::cout << "var2 type: " << typeid(var2).name() << std::endl;
    std::cout << "var3 type: " << typeid(var3).name() << std::endl;
    std::cout << "var4 type: " << typeid(var4).name() << std::endl;

    // 修改引用变量
    var2 = 20; // 修改x
    std::cout << "x after var2 = 20: " << x << std::endl;

    // 用于函数返回类型
    decltype(auto) result1 = getRef(); // int& （保留引用）
    auto result2 = getRef();           // int （复制值）

    result1 = 100; // 修改静态变量
    std::cout << "getRef() after result1 = 100: " << getRef() << std::endl;
    std::cout << "result2 value: " << result2 << std::endl; // 仍为42

    // 在函数声明中使用 - 现在使用外部函数
    std::cout << "getValue(): " << getValue() << std::endl;
    std::cout << "getRefValue(): " << getRefValue() << std::endl;
}

// 7. 二进制字面量和数字分隔符（C++14）
void useBinaryLiterals()
{
    std::cout << "\n7. Binary Literals and Digit Separators (C++14):" << std::endl;

    // 二进制字面量（C++14）
    int bin1 = 0b1010; // 十进制10
    int bin2 = 0B1100; // 十进制12

    std::cout << "0b1010: " << bin1 << std::endl;
    std::cout << "0B1100: " << bin2 << std::endl;

    // 数字分隔符（C++14）：使用'分隔数字，提高可读性
    int large1 = 1'000'000;              // 1,000,000
    long long large2 = 10'000'000'000LL; // 10,000,000,000
    double pi = 3.141'592'653'589'793;

    std::cout << "1'000'000: " << large1 << std::endl;
    std::cout << "10'000'000'000LL: " << large2 << std::endl;
    std::cout << "3.141'592'653'589'793: " << pi << std::endl;

    // 二进制数字分隔符
    int bin3 = 0b1010'1100'1110; // 二进制分隔
    std::cout << "0b1010'1100'1110: " << bin3 << std::endl;

    // 十六进制数字分隔符
    int hex = 0x1A'2B'3C'4D; // 十六进制分隔
    std::cout << "0x1A'2B'3C'4D: " << hex << std::endl;

    // 八进制数字分隔符
    int oct = 0123'456; // 八进制分隔
    std::cout << "0123'456: " << oct << std::endl;
}

int main()
{
    std::cout << "=== C++14 Features Examples ===" << std::endl;

    useAutoReturnType();
    useGenericLambda();
    useVariableTemplates();
    useLambdaInitCapture();
    useEnhancedConstexpr();
    useDecltypeAuto();
    useBinaryLiterals();

    std::cout << "\n=== C++14 Features Examples Finished ===" << std::endl;

    return 0;
}