/*
 * 知识点：枚举类
 * 源项目：e:\3.esp-brookesia\esp-brookesia\core\brookesia_core\systems\base\esp_brookesia_base_app.hpp 第78-83行
 * C++ Primer中文版（第五版）：第19章 特殊工具与技术 - 19.3 枚举类型
 * Effective+C++中文版（第三版）：无直接对应章节
 */

#include <iostream>
#include <cstdint>

// 传统枚举：存在作用域污染问题
enum AppStatus
{
    UNINSTALLED = 0,
    RUNNING,
    PAUSED,
    CLOSED
};

// 枚举类：类型安全，作用域受限
enum class ModernAppStatus : uint8_t
{
    UNINSTALLED = 0,
    RUNNING,
    PAUSED,
    CLOSED,
    MAX
};

// 另一个枚举类：不同作用域，不会冲突
enum class EventType
{
    START,
    STOP,
    OPERATION,
    MAX
};

// 结构体中使用枚举类
struct AppEventData
{
    int id;
    EventType type;
    void *data;
};

class AppManager
{
public:
    // 使用枚举类作为成员变量类型
    void setAppStatus(ModernAppStatus status)
    {
        _status = status;
        std::cout << "App status set to: " << static_cast<int>(_status) << std::endl;
    }

    ModernAppStatus getAppStatus() const
    {
        return _status;
    }

    // 使用枚举类作为函数参数和返回值
    bool handleEvent(EventType event)
    {
        std::cout << "Handling event: " << static_cast<int>(event) << std::endl;

        switch (event)
        {
        case EventType::START:
            setAppStatus(ModernAppStatus::RUNNING);
            return true;
        case EventType::STOP:
            setAppStatus(ModernAppStatus::CLOSED);
            return true;
        case EventType::OPERATION:
            return true;
        default:
            return false;
        }
    }

    // 演示枚举类比较
    bool isAppRunning() const
    {
        return _status == ModernAppStatus::RUNNING;
    }

private:
    ModernAppStatus _status = ModernAppStatus::UNINSTALLED;
};

// 传统枚举的问题演示
void traditionalEnumProblem()
{
    std::cout << "\n=== Traditional Enum Problems ===" << std::endl;

    // 问题1：作用域污染 - 枚举值直接暴露在当前作用域
    int status = RUNNING; // 可以直接使用，无需枚举名前缀
    std::cout << "Traditional enum value: " << status << std::endl;

    // 问题2：不同枚举之间可能冲突
    // 假设有另一个枚举定义了相同的枚举值
    // enum NetworkStatus { RUNNING, CONNECTED, DISCONNECTED };  // 编译错误：RUNNING 已经定义

    // 问题3：类型不安全 - 可以与其他整数类型隐式转换
    status = 100; // 可以赋值任意整数，编译不会报错
    std::cout << "Traditional enum with invalid value: " << status << std::endl;
}

// 枚举类的优势演示
void enumClassAdvantages()
{
    std::cout << "\n=== Enum Class Advantages ===" << std::endl;

    AppManager manager;

    // 优势1：必须使用作用域解析符，避免命名冲突
    manager.setAppStatus(ModernAppStatus::PAUSED);

    // 优势2：类型安全，不能直接与整数比较
    // bool isRunning = (manager.getAppStatus() == 1);  // 编译错误：类型不匹配
    bool isRunning = (manager.getAppStatus() == ModernAppStatus::RUNNING); // 正确
    std::cout << "App is running: " << (isRunning ? "true" : "false") << std::endl;

    // 优势3：可以指定底层类型，节省内存
    std::cout << "Enum class size: " << sizeof(ModernAppStatus) << " bytes" << std::endl;

    // 优势4：不同枚举类之间不会冲突
    EventType event = EventType::START;
    manager.handleEvent(event);

    // 优势5：可以安全转换为整数
    int statusValue = static_cast<int>(manager.getAppStatus());
    std::cout << "App status as int: " << statusValue << std::endl;
}

// 枚举类在结构体中的使用
void enumClassInStruct()
{
    std::cout << "\n=== Enum Class in Struct ===" << std::endl;

    AppEventData eventData;
    eventData.id = 1001;
    eventData.type = EventType::OPERATION;
    eventData.data = nullptr;

    std::cout << "Event data: " << std::endl;
    std::cout << "  ID: " << eventData.id << std::endl;
    std::cout << "  Type: " << static_cast<int>(eventData.type) << std::endl;
    std::cout << "  Data: " << eventData.data << std::endl;
}

int main()
{
    traditionalEnumProblem();
    enumClassAdvantages();
    enumClassInStruct();

    return 0;
}