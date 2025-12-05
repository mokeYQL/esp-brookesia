// ============================================================================//
// ESP-Brookesia 项目 C++ 学习文件 02_oop.cpp
// 面向对象编程特性：继承、多态、虚函数、函数覆盖、静态成员、枚举类
// ============================================================================//

#include <iostream>
#include <string>
#include <vector>

using namespace std;

// =========================================
// 1. 继承（Inheritance）
// =========================================
// 继承是面向对象编程的核心特性，用于代码复用和扩展
// 学习要点：单继承、多继承、访问控制、构造函数调用顺序

// 示例1：单继承基础
class BaseApp
{
private:
    int _app_id;
    string _app_name;

protected:
    // 受保护成员：派生类可访问
    bool _is_running;

public:
    // 静态成员：类级别的常量
    static const int APP_ID_MIN = 1000;
    static const int APP_ID_MAX = 9999;

    // 构造函数
    BaseApp(int app_id, const string &name)
        : _app_id(app_id), _app_name(name), _is_running(false)
    {
        cout << "BaseApp constructor called: " << name << endl;
    }

    // 虚析构函数：用于多态场景下的正确析构
    virtual ~BaseApp()
    {
        cout << "BaseApp destructor called: " << _app_name << endl;
    }

    // 普通成员函数
    void start()
    {
        _is_running = true;
        cout << _app_name << " started" << endl;
    }

    void stop()
    {
        _is_running = false;
        cout << _app_name << " stopped" << endl;
    }

    bool is_running() const
    {
        return _is_running;
    }

    // 虚函数：用于实现多态
    virtual void update()
    {
        cout << _app_name << " base update called" << endl;
    }

    // 纯虚函数：抽象方法，派生类必须实现
    virtual bool run() = 0;

    // 删除函数：禁用默认拷贝构造和赋值操作
    BaseApp(const BaseApp &) = delete;
    BaseApp &operator=(const BaseApp &) = delete;

    // 获取应用信息
    string get_app_info() const
    {
        return _app_name + " (ID: " + to_string(_app_id) + ")";
    }
};

// 示例2：单继承派生类
class Calculator : public BaseApp
{
private:
    double _result;
    string _last_operation;

public:
    // 派生类构造函数：必须调用基类构造函数
    Calculator(int app_id)
        : BaseApp(app_id, "Calculator"), _result(0.0), _last_operation("none")
    {
        cout << "Calculator constructor called" << endl;
    }

    // 派生类析构函数
    ~Calculator()
    {
        cout << "Calculator destructor called" << endl;
    }

    // 覆盖基类的update虚函数
    void update() override
    {
        cout << "Calculator update: last operation = " << _last_operation
             << ", result = " << _result << endl;
    }

    // 实现纯虚函数run
    bool run() override
    {
        cout << "Calculator is running..." << endl;
        start();

        // 模拟计算器运行
        add(10.5);
        multiply(2);
        subtract(5.5);
        update();

        stop();
        return true;
    }

    // 计算器特定方法
    void add(double value)
    {
        _result += value;
        _last_operation = "add";
    }

    void subtract(double value)
    {
        _result -= value;
        _last_operation = "subtract";
    }

    void multiply(double value)
    {
        _result *= value;
        _last_operation = "multiply";
    }

    void divide(double value)
    {
        if (value != 0)
        {
            _result /= value;
            _last_operation = "divide";
        }
    }

    double get_result() const
    {
        return _result;
    }
};

// 示例3：另一个派生类
class MusicPlayer : public BaseApp
{
private:
    string _current_song;
    int _volume;

public:
    MusicPlayer(int app_id)
        : BaseApp(app_id, "MusicPlayer"), _volume(50), _current_song("none")
    {
        cout << "MusicPlayer constructor called" << endl;
    }

    ~MusicPlayer()
    {
        cout << "MusicPlayer destructor called" << endl;
    }

    // 覆盖基类的update虚函数
    void update() override
    {
        cout << "MusicPlayer update: playing " << _current_song
             << " at volume " << _volume << endl;
    }

    // 实现纯虚函数run
    bool run() override
    {
        cout << "MusicPlayer is running..." << endl;
        start();

        // 模拟音乐播放器运行
        play_song("Shape of You");
        set_volume(70);
        update();

        stop();
        return true;
    }

    // 音乐播放器特定方法
    void play_song(const string &song)
    {
        _current_song = song;
        cout << "Now playing: " << song << endl;
    }

    void set_volume(int volume)
    {
        if (volume >= 0 && volume <= 100)
        {
            _volume = volume;
        }
    }

    string get_current_song() const
    {
        return _current_song;
    }
};

// 示例4：多继承（注意：多继承应谨慎使用，容易引发菱形继承问题）
class BluetoothDevice
{
protected:
    bool _is_connected;

public:
    BluetoothDevice() : _is_connected(false)
    {
        cout << "BluetoothDevice constructor called" << endl;
    }

    virtual ~BluetoothDevice()
    {
        cout << "BluetoothDevice destructor called" << endl;
    }

    bool connect()
    {
        _is_connected = true;
        cout << "Bluetooth connected" << endl;
        return true;
    }

    void disconnect()
    {
        _is_connected = false;
        cout << "Bluetooth disconnected" << endl;
    }

    bool is_connected() const
    {
        return _is_connected;
    }
};

// 多继承示例：SmartSpeaker同时继承自BaseApp和BluetoothDevice
class SmartSpeaker : public BaseApp, public BluetoothDevice
{
private:
    string _voice_assistant;

public:
    SmartSpeaker(int app_id)
        : BaseApp(app_id, "SmartSpeaker"), BluetoothDevice(), _voice_assistant("Alexa")
    {
        cout << "SmartSpeaker constructor called" << endl;
    }

    ~SmartSpeaker()
    {
        cout << "SmartSpeaker destructor called" << endl;
    }

    // 覆盖基类的update虚函数
    void update() override
    {
        cout << "SmartSpeaker update: voice assistant = " << _voice_assistant
             << ", bluetooth connected = " << (is_connected() ? "yes" : "no") << endl;
    }

    // 实现纯虚函数run
    bool run() override
    {
        cout << "SmartSpeaker is running..." << endl;
        start();
        connect();
        update();
        disconnect();
        stop();
        return true;
    }

    void set_voice_assistant(const string &assistant)
    {
        _voice_assistant = assistant;
    }
};

// =========================================
// 2. 多态（Polymorphism）
// =========================================
// 多态允许使用基类指针或引用来操作派生类对象
// 学习要点：动态绑定、虚函数表、基类指针数组

// 示例1：使用基类指针实现多态
void demonstrate_polymorphism()
{
    cout << "\n=== 演示多态特性 ===" << endl;

    // 创建不同类型的应用对象，使用基类指针存储
    vector<BaseApp *> apps;
    apps.push_back(new Calculator(1001));
    apps.push_back(new MusicPlayer(1002));
    apps.push_back(new SmartSpeaker(1003));

    // 使用基类指针调用虚函数，实现动态绑定
    for (auto app : apps)
    {
        cout << "\n--- " << app->get_app_info() << " ---" << endl;
        app->run();    // 调用派生类的run方法
        app->update(); // 调用派生类的update方法
    }

    // 释放资源
    for (auto app : apps)
    {
        delete app; // 调用正确的析构函数链
    }
}

// 示例2：使用基类引用实现多态
void process_app(BaseApp &app)
{
    cout << "\nProcessing app: " << app.get_app_info() << endl;
    app.run();
    app.update();
}

// =========================================
// 3. 枚举类（Enum Class）
// =========================================
// 枚举类是类型安全的枚举，避免了传统枚举的隐式转换问题
// 学习要点：枚举类的定义、使用、类型安全

// 示例1：基础枚举类
enum class Status : uint8_t
{
    IDLE = 0,
    RUNNING = 1,
    PAUSED = 2,
    ERROR = 3,
    STOPPED = 4
};

// 示例2：应用事件类型枚举类
enum class AppEventType : uint8_t
{
    CREATE = 0,
    START = 1,
    UPDATE = 2,
    STOP = 3,
    DESTROY = 4,
    CUSTOM = 5
};

// 示例3：枚举类的使用
void process_status(Status status) {
    switch (status) {
        case Status::IDLE:
            cout << "Status: IDLE" << endl;
            break;
        case Status::RUNNING:
            cout << "Status: RUNNING" << endl;
            break;
        case Status::PAUSED:
            cout << "Status: PAUSED" << endl;
            break;
        case Status::ERROR:
            cout << "Status: ERROR" << endl;
            break;
        case Status::STOPPED:
            cout << "Status: STOPPED" << endl;
            break;
        default:
            cout << "Status: UNKNOWN" << endl;
            break;
    }
}

// =========================================
// 4. 静态成员（Static Members）
// =========================================
// 静态成员属于类，而非对象实例
// 学习要点：静态成员变量、静态成员函数

class AppRegistry {
private:
    // 静态成员变量：所有对象共享
    static int _app_count;
    static const string _registry_name;
    
    string _instance_id;
    
public:
    AppRegistry() {
        _app_count++;
        _instance_id = "reg_" + to_string(_app_count);
        cout << "AppRegistry instance created: " << _instance_id << endl;
    }
    
    ~AppRegistry() {
        _app_count--;
        cout << "AppRegistry instance destroyed: " << _instance_id << endl;
    }
    
    // 静态成员函数：只能访问静态成员
    static int get_app_count() {
        return _app_count;
    }
    
    static string get_registry_name() {
        return _registry_name;
    }
    
    // 普通成员函数：可以访问静态和非静态成员
    string get_instance_info() const {
        return _instance_id + " (" + _registry_name + ")";
    }
};

// 静态成员变量的初始化（必须在类外进行）
int AppRegistry::_app_count = 0;
const string AppRegistry::_registry_name = "ESP-Brookesia Registry";

// =========================================
// 5. 函数重载与函数覆盖
// =========================================
// 函数重载：同一作用域内，函数名相同但参数列表不同
// 函数覆盖：派生类重新实现基类的虚函数

class OverloadExample {
private:
    int _value;
    
public:
    OverloadExample(int value) : _value(value) {}
    
    // 函数重载示例：同名函数，不同参数
    void print() const {
        cout << "Value: " << _value << endl;
    }
    
    void print(int multiplier) const {
        cout << "Value * " << multiplier << " = " << _value * multiplier << endl;
    }
    
    void print(const string& prefix) const {
        cout << prefix << ": " << _value << endl;
    }
    
    void print(const string& prefix, int multiplier) const {
        cout << prefix << ": " << _value * multiplier << endl;
    }
};

// =========================================
// 6. 抽象基类与接口
// =========================================
// 抽象基类包含纯虚函数，不能直接实例化
// 接口：只包含纯虚函数的抽象基类

// 示例：图形接口
class Shape {
public:
    virtual ~Shape() = default;
    
    // 纯虚函数：接口方法
    virtual double get_area() const = 0;
    virtual double get_perimeter() const = 0;
    virtual void draw() const = 0;
    virtual string get_type() const = 0;
};

// 圆形类实现Shape接口
class Circle : public Shape {
private:
    double _radius;
    
public:
    Circle(double radius) : _radius(radius) {}
    
    double get_area() const override {
        return 3.14159 * _radius * _radius;
    }
    
    double get_perimeter() const override {
        return 2 * 3.14159 * _radius;
    }
    
    void draw() const override {
        cout << "Drawing a circle with radius " << _radius << endl;
    }
    
    string get_type() const override {
        return "Circle";
    }
};

// 矩形类实现Shape接口
class Rectangle : public Shape {
private:
    double _width;
    double _height;
    
public:
    Rectangle(double width, double height) : _width(width), _height(height) {}
    
    double get_area() const override {
        return _width * _height;
    }
    
    double get_perimeter() const override {
        return 2 * (_width + _height);
    }
    
    void draw() const override {
        cout << "Drawing a rectangle with width " << _width 
             << " and height " << _height << endl;
    }
    
    string get_type() const override {
        return "Rectangle";
    }
};

// =========================================
// 主函数：测试所有面向对象特性
// =========================================
int main() {
    cout << "=========================================" << endl;
    cout << "ESP-Brookesia C++ 面向对象编程学习" << endl;
    cout << "=========================================" << endl;
    
    // =========================================
    // 测试1：继承与构造函数调用顺序
    cout << "\n1. 测试继承与构造函数调用顺序：" << endl;
    cout << "-----------------------------------------" << endl;
    Calculator calc(1001);
    cout << "\nCalculator app info: " << calc.get_app_info() << endl;
    
    // =========================================
    // 测试2：多态特性
    cout << "\n2. 测试多态特性：" << endl;
    cout << "-----------------------------------------" << endl;
    demonstrate_polymorphism();
    
    // =========================================
    // 测试3：基类引用多态
    cout << "\n3. 测试基类引用多态：" << endl;
    cout << "-----------------------------------------" << endl;
    MusicPlayer player(1002);
    process_app(player);
    
    // =========================================
    // 测试4：枚举类
    cout << "\n4. 测试枚举类：" << endl;
    cout << "-----------------------------------------" << endl;
    Status status1 = Status::RUNNING;
    Status status2 = Status::PAUSED;
    process_status(status1);
    process_status(status2);
    
    // 枚举类的类型安全特性（编译错误示例）
    // int status_int = status1; // 错误：不能隐式转换枚举类到int
    int status_int = static_cast<int>(status1); // 正确：需要显式转换
    cout << "Status::RUNNING as int: " << status_int << endl;
    
    // =========================================
    // 测试5：静态成员
    cout << "\n5. 测试静态成员：" << endl;
    cout << "-----------------------------------------" << endl;
    cout << "Initial app count: " << AppRegistry::get_app_count() << endl;
    cout << "Registry name: " << AppRegistry::get_registry_name() << endl;
    
    AppRegistry reg1;
    AppRegistry reg2;
    AppRegistry reg3;
    
    cout << "After creating 3 instances, app count: " << AppRegistry::get_app_count() << endl;
    cout << "reg1 info: " << reg1.get_instance_info() << endl;
    cout << "reg2 info: " << reg2.get_instance_info() << endl;
    
    { // 作用域测试
        AppRegistry reg4;
        cout << "Inside scope, app count: " << AppRegistry::get_app_count() << endl;
    } // reg4超出作用域被销毁
    
    cout << "Outside scope, app count: " << AppRegistry::get_app_count() << endl;
    
    // =========================================
    // 测试6：函数重载
    cout << "\n6. 测试函数重载：" << endl;
    cout << "-----------------------------------------" << endl;
    OverloadExample overload(42);
    overload.print();
    overload.print(2);
    overload.print("Prefix");
    overload.print("Prefixed multiplied", 3);
    
    // =========================================
    // 测试7：抽象基类与接口
    cout << "\n7. 测试抽象基类与接口：" << endl;
    cout << "-----------------------------------------" << endl;
    vector<Shape*> shapes;
    shapes.push_back(new Circle(5.0));
    shapes.push_back(new Rectangle(4.0, 6.0));
    shapes.push_back(new Circle(3.0));
    
    for (auto shape : shapes) {
        cout << "\n--- " << shape->get_type() << " ---" << endl;
        shape->draw();
        cout << "Area: " << shape->get_area() << endl;
        cout << "Perimeter: " << shape->get_perimeter() << endl;
    }
    
    // 释放资源
    for (auto shape : shapes) {
        delete shape;
    }
    
    cout << "\n=========================================" << endl;
    cout << "面向对象编程学习完成！" << endl;
    cout << "=========================================" << endl;
    
    return 0;
}

// ============================================================================//
// 代码学习要点总结
// ============================================================================//
/*
1. 继承：
   - 单继承：一个派生类继承自一个基类
   - 多继承：一个派生类继承自多个基类（需谨慎使用）
   - 构造函数调用顺序：基类构造函数先于派生类构造函数执行
   - 析构函数调用顺序：派生类析构函数先于基类析构函数执行
   - 访问控制：private、protected、public继承方式影响基类成员在派生类中的访问权限

2. 多态：
   - 动态绑定：在运行时根据对象的实际类型调用相应的虚函数
   - 实现条件：基类有虚函数，派生类覆盖该虚函数，使用基类指针或引用
   - 虚函数表：编译器为包含虚函数的类生成虚函数表，实现动态绑定

3. 虚函数与纯虚函数：
   - 虚函数：使用virtual关键字声明，派生类可选择覆盖
   - 纯虚函数：virtual 返回类型 函数名() = 0; 必须在派生类中实现
   - 抽象基类：包含纯虚函数的类，不能直接实例化
   - 接口：只包含纯虚函数和静态常量的抽象基类

4. 虚析构函数：
   - 必须为多态基类声明虚析构函数，避免内存泄漏
   - 确保删除基类指针时，正确调用派生类析构函数

5. 枚举类：
   - 类型安全：不能隐式转换为int
   - 作用域：枚举值被限制在枚举类作用域内
   - 可指定底层类型：enum class EnumName : type { ... };

6. 静态成员：
   - 静态成员变量：所有类实例共享，必须在类外初始化
   - 静态成员函数：只能访问静态成员，不能使用this指针
   - 用于实现类级别的功能和数据共享

7. 函数重载与覆盖：
   - 函数重载：同一作用域内，同名函数不同参数列表
   - 函数覆盖：派生类重新实现基类的虚函数，使用override关键字标识
   - override关键字：提高代码可读性，防止意外重载

8. 删除函数：
   - 使用= delete禁用不需要的默认函数（如拷贝构造、赋值操作）
   - 提高代码安全性，明确禁止某些操作
*/
