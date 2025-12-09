/*
 * 知识点：C++11更多特性（nullptr、 constexpr、decltype、default/delete）
 * 源项目：e:\3.esp-brookesia\esp-brookesia\core\brookesia_core\systems\base\esp_brookesia_base_app.hpp 第92-95行
 * C++ Primer中文版（第五版）：第12章 动态内存 - 12.1 动态内存与智能指针
 * Effective+C++中文版（第三版）：无直接对应章节
 */

#include <iostream>
#include <string>
#include <vector>
#include <typeinfo>

// 1. nullptr 空指针常量
// 函数重载示例（移到函数外部）
void func(int)
{
    std::cout << "func(int) called" << std::endl;
}

void func(void *)
{
    std::cout << "func(void*) called" << std::endl;
}

void useNullptr()
{
    std::cout << "\n1. nullptr Example:" << std::endl;

    // 传统方式：使用 NULL 宏或 0
    int *ptr1 = NULL; // NULL 可能被定义为 0
    int *ptr2 = 0;    // 字面量 0

    // C++11方式：使用 nullptr
    int *ptr3 = nullptr; // 类型安全的空指针
    (void)ptr3;          // 避免未使用警告

    std::cout << "ptr1 is nullptr: " << (ptr1 == nullptr ? "true" : "false") << std::endl;
    std::cout << "ptr2 is nullptr: " << (ptr2 == nullptr ? "true" : "false") << std::endl;

    func(0); // 调用 func(int)
    // func(NULL);       // 注释掉，因为 NULL 会导致二义性错误
    func(static_cast<void *>(NULL)); // 显式转换为 void*，调用 func(void*)
    func(nullptr);                   // 调用 func(void*)，类型安全
}

// 2. constexpr 常量表达式
// 常量表达式函数（移到函数外部）
constexpr int square(int x)
{
    return x * x;
}

// 编译期计算函数（移到函数外部）
constexpr bool isEven(int n)
{
    return (n % 2 == 0);
}

// 常量表达式结构体（移到函数外部）
struct Point
{
    int x;
    int y;

    // 常量表达式构造函数
    constexpr Point(int x_val, int y_val) : x(x_val), y(y_val) {}

    // 常量表达式成员函数
    constexpr int getArea() const
    {
        return x * y;
    }
};

void useConstexpr()
{
    std::cout << "\n2. constexpr Example:" << std::endl;

    // 常量表达式变量
    constexpr int MAX_APPS = 10;
    constexpr int SCREEN_WIDTH = 320;
    constexpr int SCREEN_HEIGHT = 240;
    constexpr int SCREEN_AREA = SCREEN_WIDTH * SCREEN_HEIGHT;

    std::cout << "MAX_APPS: " << MAX_APPS << std::endl;
    std::cout << "SCREEN_AREA: " << SCREEN_AREA << std::endl;

    constexpr int SQUARE_5 = square(5);
    constexpr int SQUARE_10 = square(10);

    std::cout << "square(5): " << SQUARE_5 << std::endl;
    std::cout << "square(10): " << SQUARE_10 << std::endl;

    constexpr bool IS_EVEN_10 = isEven(10);
    constexpr bool IS_EVEN_11 = isEven(11);

    std::cout << "isEven(10): " << (IS_EVEN_10 ? "true" : "false") << std::endl;
    std::cout << "isEven(11): " << (IS_EVEN_11 ? "true" : "false") << std::endl;

    constexpr Point p1(10, 20);
    constexpr int area = p1.getArea();

    std::cout << "Point p1: (" << p1.x << ", " << p1.y << "), area: " << area << std::endl;
}

// 3. decltype 类型推导
// C++14：decltype(auto) 推导返回值类型（移到函数外部）
auto getValue() -> decltype(auto)
{
    static int value = 42;
    return value; // 返回 int&
}

void useDecltype()
{
    std::cout << "\n3. decltype Example:" << std::endl;

    // 推导变量类型
    int x = 10;
    decltype(x) y = 20; // y 的类型为 int

    double d = 3.14;
    decltype(d) e = 2.71; // e 的类型为 double

    std::cout << "y type: " << typeid(y).name() << ", y value: " << y << std::endl;
    std::cout << "e type: " << typeid(e).name() << ", e value: " << e << std::endl;

    // 推导表达式类型
    decltype(x + d) f = x + d; // f 的类型为 double
    std::cout << "x + d type: " << typeid(f).name() << ", value: " << f << std::endl;

    // 推导函数返回值类型
    auto add = [](int a, double b) -> double
    {
        return a + b;
    };

    decltype(add(10, 20.5)) result = add(10, 20.5);
    std::cout << "add(10, 20.5) type: " << typeid(result).name() << ", value: " << result << std::endl;

    // 与 auto 结合使用
    std::vector<int> vec = {1, 2, 3, 4, 5};
    for (decltype(vec.begin()) it = vec.begin(); it != vec.end(); ++it)
    {
        std::cout << *it << " ";
    }
    std::cout << std::endl;

    decltype(auto) ref = getValue();
    ref = 100; // 修改静态变量
    std::cout << "getValue() after modification: " << getValue() << std::endl;
}

// 4. default 和 delete 函数
class DefaultDeleteExample
{
public:
    // 默认构造函数
    DefaultDeleteExample() = default;

    // 带参数的构造函数
    explicit DefaultDeleteExample(const std::string &name) : _name(name)
    {
        std::cout << "DefaultDeleteExample constructor: " << _name << std::endl;
    }

    // 禁止拷贝构造函数
    DefaultDeleteExample(const DefaultDeleteExample &) = delete;

    // 禁止拷贝赋值运算符
    DefaultDeleteExample &operator=(const DefaultDeleteExample &) = delete;

    // 默认移动构造函数
    DefaultDeleteExample(DefaultDeleteExample &&) = default;

    // 默认移动赋值运算符
    DefaultDeleteExample &operator=(DefaultDeleteExample &&) = default;

    // 默认析构函数
    ~DefaultDeleteExample() = default;

    std::string getName() const
    {
        return _name;
    }

private:
    std::string _name;
};

void useDefaultDelete()
{
    std::cout << "\n4. default and delete Example:" << std::endl;

    DefaultDeleteExample obj1;
    DefaultDeleteExample obj2("test");

    std::cout << "obj1 name: " << obj1.getName() << std::endl;
    std::cout << "obj2 name: " << obj2.getName() << std::endl;

    // 移动语义可用
    DefaultDeleteExample obj3 = std::move(obj2);
    std::cout << "obj3 name after move: " << obj3.getName() << std::endl;

    // 以下操作会编译错误
    // DefaultDeleteExample obj4 = obj1;          // 拷贝构造被删除
    // obj1 = obj3;                              // 拷贝赋值被删除
}

// 5. 列表初始化（统一初始化）
void useListInitialization()
{
    std::cout << "\n5. List Initialization Example:" << std::endl;

    // 基本类型列表初始化
    int x1 = 10;   // 传统赋值初始化
    int x2(20);    // 构造函数初始化
    int x3{30};    // 列表初始化（C++11）
    int x4 = {40}; // 列表初始化（C++11）

    std::cout << "x1: " << x1 << ", x2: " << x2 << ", x3: " << x3 << ", x4: " << x4 << std::endl;

    // 防止窄化转换
    // int x5{3.14};       // 编译错误：double 到 int 的窄化转换

    // 容器列表初始化
    std::vector<int> vec1{1, 2, 3, 4, 5}; // 直接初始化容器
    std::vector<int> vec2 = {6, 7, 8, 9, 10};

    std::cout << "vec1 elements: ";
    for (const auto &elem : vec1)
    {
        std::cout << elem << " ";
    }
    std::cout << std::endl;

    // 结构体列表初始化
    struct Point
    {
        int x;
        int y;
    };

    Point p1{10, 20};    // 列表初始化结构体
    Point p2 = {30, 40}; // 列表初始化结构体

    std::cout << "Point p1: (" << p1.x << ", " << p1.y << ")" << std::endl;
    std::cout << "Point p2: (" << p2.x << ", " << p2.y << ")" << std::endl;

    // 类列表初始化
    class App
    {
    public:
        App(int id, const std::string &name) : _id(id), _name(name) {}

        void display() const
        {
            std::cout << "App: id=" << _id << ", name=" << _name << std::endl;
        }

    private:
        int _id;
        std::string _name;
    };

    App app{1, "Calculator"}; // 列表初始化类
    app.display();
}

// 6. alias 模板（模板别名）
// 传统方式：使用 typedef
typedef std::vector<int> IntVector;

// C++11方式：使用 using（推荐）- 移到全局作用域
template <typename T>
using Vec = std::vector<T>;

// 嵌套容器模板别名 - 移到全局作用域
template <typename T>
using Matrix = std::vector<std::vector<T>>;

void useAliasTemplate()
{
    std::cout << "\n6. Alias Template Example:" << std::endl;

    IntVector vec1 = {1, 2, 3};
    Vec<int> vec2 = {4, 5, 6};
    Vec<std::string> vec3 = {"apple", "banana", "cherry"};

    std::cout << "vec1 size: " << vec1.size() << std::endl;
    std::cout << "vec2 size: " << vec2.size() << std::endl;
    std::cout << "vec3 size: " << vec3.size() << std::endl;

    Matrix<int> matrix = {
        {1, 2, 3},
        {4, 5, 6},
        {7, 8, 9}};

    std::cout << "Matrix dimensions: " << matrix.size() << "x" << matrix[0].size() << std::endl;
    for (const auto &row : matrix)
    {
        for (const auto &elem : row)
        {
            std::cout << elem << " ";
        }
        std::cout << std::endl;
    }
}

// 7. noexcept 异常说明
class NoexceptExample
{
public:
    NoexceptExample() noexcept
    {
        std::cout << "NoexceptExample constructor" << std::endl;
    }

    ~NoexceptExample() noexcept
    {
        std::cout << "NoexceptExample destructor" << std::endl;
    }

    // noexcept 成员函数
    void safeFunction() noexcept
    {
        std::cout << "safeFunction() - noexcept" << std::endl;
        // 不会抛出异常
    }

    // 可能抛出异常的函数
    void unsafeFunction()
    {
        std::cout << "unsafeFunction() - may throw" << std::endl;
        // 可能抛出异常
    }

    // 条件 noexcept
    template <typename T>
    void conditionalNoexcept(const T &value) noexcept(noexcept(value.toString()))
    {
        // 仅当 value.toString() 是 noexcept 时，该函数才是 noexcept
        std::cout << "conditionalNoexcept()" << std::endl;
    }
};

void useNoexcept()
{
    std::cout << "\n7. noexcept Example:" << std::endl;

    NoexceptExample obj;
    obj.safeFunction();
    obj.unsafeFunction();

    // 检查函数是否是 noexcept
    std::cout << "safeFunction is noexcept: " << noexcept(obj.safeFunction()) << std::endl;
    std::cout << "unsafeFunction is noexcept: " << noexcept(obj.unsafeFunction()) << std::endl;

    // 移动构造函数建议使用 noexcept
    std::vector<NoexceptExample> vec;
    vec.reserve(10);
    vec.emplace_back(); // 调用 noexcept 构造函数，更高效
}

// 8. override 和 final 关键字
class Base
{
public:
    virtual ~Base() = default;

    // 虚函数
    virtual void func()
    {
        std::cout << "Base::func()" << std::endl;
    }

    // 虚函数
    virtual void anotherFunc()
    {
        std::cout << "Base::anotherFunc()" << std::endl;
    }

    // 非虚函数
    void nonVirtualFunc()
    {
        std::cout << "Base::nonVirtualFunc()" << std::endl;
    }
};

class Derived : public Base
{
public:
    // override 确保覆盖基类虚函数
    void func() override
    {
        std::cout << "Derived::func() - overridden" << std::endl;
    }

    // final 禁止子类进一步覆盖
    void anotherFunc() final
    {
        std::cout << "Derived::anotherFunc() - final" << std::endl;
    }

    // 以下操作会编译错误
    // void nonVirtualFunc() override;  // 非虚函数不能被覆盖
};

// 以下类会编译错误，因为 anotherFunc 被标记为 final
// class Derived2 : public Derived {
// public:
//     void anotherFunc() override;  // 编译错误：anotherFunc 是 final
// };

void useOverrideFinal()
{
    std::cout << "\n8. override and final Example:" << std::endl;

    Base *base = new Derived();
    base->func();           // 调用 Derived::func()
    base->anotherFunc();    // 调用 Derived::anotherFunc()
    base->nonVirtualFunc(); // 调用 Base::nonVirtualFunc()

    delete base;
}

int main()
{
    std::cout << "=== C++11 More Features Examples ===" << std::endl;

    useNullptr();
    useConstexpr();
    useDecltype();
    useDefaultDelete();
    useListInitialization();
    useAliasTemplate();
    useNoexcept();
    useOverrideFinal();

    std::cout << "\n=== C++11 More Features Examples Finished ===" << std::endl;

    return 0;
}