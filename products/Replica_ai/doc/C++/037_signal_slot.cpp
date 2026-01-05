/*
 * 知识点：信号槽机制
 * 源项目：e:\3.esp-brookesia\esp-brookesia\products\speaker\main\modules\system.cpp 第158-190行
 * C++ Primer中文版（第五版）：第10章 泛型算法 - 10.3 定制操作
 * Effective+C++中文版（第三版）：条款21 必须返回对象时，别妄想返回其reference
 */
#include <iostream>
#include <functional>
#include <vector>
#include <string>

// 简单的信号槽实现
class Signal {
public:
    using Slot = std::function<bool(int)>;
    
    void connect(Slot slot) {
        slots.push_back(slot);
    }
    
    bool emit(int value) {
        for (auto& slot : slots) {
            if (!slot(value)) {
                return false;
            }
        }
        return true;
    }
    
private:
    std::vector<Slot> slots;
};

class EventData {
public:
    enum class EventType {
        ValueChanged,
        ErrorOccurred
    };
    
    EventData(EventType type, int value) : type(type), value(value) {}
    
    EventType type;
    int value;
};

class ValueNotifier {
public:
    Signal& getSignal() {
        return value_changed_signal;
    }
    
    void setValue(int new_value) {
        if (value != new_value) {
            value = new_value;
            std::cout << "Value changed to: " << value << std::endl;
            value_changed_signal.emit(value);
        }
    }
    
    int getValue() const { return value; }
    
private:
    int value = 0;
    Signal value_changed_signal;
};

class ValueLogger {
public:
    ValueLogger(const std::string& name) : name(name) {}
    
    bool onValueChanged(int value) {
        std::cout << name << ": Value updated to " << value << std::endl;
        return true;
    }
    
private:
    std::string name;
};

class ValueValidator {
public:
    bool onValueChanged(int value) {
        if (value < 0 || value > 100) {
            std::cout << "Validator: Invalid value " << value << " (must be 0-100)" << std::endl;
            return false;
        }
        std::cout << "Validator: Value " << value << " is valid" << std::endl;
        return true;
    }
};

int main() {
    ValueNotifier notifier;
    ValueLogger logger("Logger");
    ValueValidator validator;
    
    // 连接信号和槽
    notifier.getSignal().connect([&logger](int value) {
        return logger.onValueChanged(value);
    });
    
    notifier.getSignal().connect([&validator](int value) {
        return validator.onValueChanged(value);
    });
    
    // 连接一个lambda作为槽
    notifier.getSignal().connect([](int value) {
        if (value > 80) {
            std::cout << "Lambda: Warning - value is high!" << std::endl;
        }
        return true;
    });
    
    // 测试不同的值
    std::cout << "=== Testing signal-slot mechanism ===" << std::endl;
    notifier.setValue(50);
    notifier.setValue(90);
    notifier.setValue(-10); // 这个值会被验证器拒绝
    notifier.setValue(75);
    
    return 0;
}
