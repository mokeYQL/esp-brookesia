/*
 * 知识点：dynamic_cast动态类型转换
 * 源项目：e:\3.esp-brookesia\esp-brookesia\products\speaker\main\modules\system.cpp 第125-126行
 * C++ Primer中文版（第五版）：第15章 面向对象程序设计 - 15.2 基类与派生类 - 15.2.3 虚函数与动态绑定
 * Effective+C++中文版（第三版）：条款37 绝不重新定义继承而来的缺省参数值
 */
#include <iostream>
#include <memory>
#include <string>
#include <vector>

class App
{
public:
    App(const std::string &name, int id) : name(name), id(id) {}
    virtual ~App() = default;

    virtual std::string getType() const { return "BaseApp"; }
    int getId() const { return id; }
    std::string getName() const { return name; }

private:
    std::string name;
    int id;
};

class Settings : public App
{
public:
    Settings(int id) : App("Settings", id) {}

    std::string getType() const override { return "SettingsApp"; }

    void openSettings()
    {
        std::cout << "Opening settings..." << std::endl;
    }

    void updateAboutInfo()
    {
        std::cout << "Updating about information..." << std::endl;
    }
};

class Calculator : public App
{
public:
    Calculator(int id) : App("Calculator", id) {}

    std::string getType() const override { return "CalculatorApp"; }

    int add(int a, int b)
    {
        return a + b;
    }
};

int main()
{
    // 创建应用列表
    std::vector<std::shared_ptr<App>> apps;
    apps.push_back(std::make_shared<Settings>(1));
    apps.push_back(std::make_shared<Calculator>(2));
    apps.push_back(std::make_shared<App>("Unknown", 3));

    Settings *app_settings = nullptr;

    // 遍历应用列表，找到Settings类型的应用
    for (const auto &app : apps)
    {
        std::cout << "Checking app: " << app->getName() << " (Type: " << app->getType() << ")" << std::endl;

        // 使用dynamic_cast将基类指针转换为派生类指针
        auto settings_ptr = std::dynamic_pointer_cast<Settings>(app);
        if (settings_ptr)
        {
            std::cout << "Found Settings app with ID: " << settings_ptr->getId() << std::endl;
            app_settings = settings_ptr.get();

            // 调用Settings特有的方法
            app_settings->openSettings();
            app_settings->updateAboutInfo();
        }

        // 测试转换失败的情况
        auto calculator_ptr = std::dynamic_pointer_cast<Calculator>(app);
        if (calculator_ptr)
        {
            int result = calculator_ptr->add(10, 20);
            std::cout << "Calculator result: 10 + 20 = " << result << std::endl;
        }
    }

    return 0;
}
