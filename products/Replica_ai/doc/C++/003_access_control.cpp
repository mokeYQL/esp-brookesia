/*
 * 知识点：访问控制
 * 源项目：e:\3.esp-brookesia\esp-brookesia\core\brookesia_core\systems\base\esp_brookesia_base_app.hpp 第24-472行
 * C++ Primer中文版（第五版）：第7章 类 - 7.2 访问控制与封装
 * Effective+C++中文版（第三版）：条款22 声明为private继承意味什么
 */

#include <iostream>
#include <string>

class BaseApp
{
public:
    // 公有成员：外部可访问
    BaseApp(const std::string &name) : _name(name)
    {
        _status = Status::UNINSTALLED;
    }

    // 公有方法
    void install()
    {
        _status = Status::INSTALLED;
        std::cout << _name << " installed" << std::endl;
    }

    void run()
    {
        if (_status == Status::INSTALLED)
        {
            _status = Status::RUNNING;
            std::cout << _name << " running" << std::endl;
        }
    }

    // 公有访问器
    std::string getName() const
    {
        return _name;
    }

protected:
    // 保护成员：子类可访问，外部不可访问
    enum class Status
    {
        UNINSTALLED,
        INSTALLED,
        RUNNING,
        PAUSED,
        CLOSED
    };

    Status getStatus() const
    {
        return _status;
    }

private:
    // 私有成员：只有类内部可访问
    std::string _name;
    Status _status;
    int _id = 0;
};

// 子类继承
class CalculatorApp : public BaseApp
{
public:
    CalculatorApp() : BaseApp("Calculator")
    {
        // 可以访问父类的保护成员
        std::cout << "CalculatorApp created, status: " << static_cast<int>(getStatus()) << std::endl;
    }
    
    void calculate(int a, int b) {
        std::cout << "Result: " << (a + b) << std::endl;
    }
};

int main() {
    CalculatorApp calc;
    calc.install();
    calc.run();
    calc.calculate(10, 20);
    
    // 可以访问公有成员
    std::cout << "App name: " << calc.getName() << std::endl;
    
    // 下面的代码会编译错误，因为_status是私有成员
    // std::cout << "Status: " << calc._status << std::endl;
    
    return 0;
}