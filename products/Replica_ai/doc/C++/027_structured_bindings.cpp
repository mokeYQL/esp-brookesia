/*
 * 知识点：结构化绑定
 * 源项目：e:\3.esp-brookesia\esp-brookesia\products\speaker\main\modules\system.cpp 第121-122行
 * C++ Primer中文版（第五版）：第7章 类 - 7.3.2 类的作用域
 * Effective+C++中文版（第三版）：无直接对应章节
 */
#include <iostream>
#include <vector>
#include <string>
#include <tuple>
#include <memory>

class App
{
public:
    App(int id) : id(id) {}
    int getId() const { return id; }

private:
    int id;
};

int main()
{
    // 创建包含键值对的vector
    std::vector<std::tuple<std::string, std::shared_ptr<App>>> inited_apps;

    // 添加一些应用
    inited_apps.emplace_back("Settings", std::make_shared<App>(1));
    inited_apps.emplace_back("Calculator", std::make_shared<App>(2));
    inited_apps.emplace_back("Timer", std::make_shared<App>(3));

    // 使用结构化绑定遍历vector中的tuple
    for (const auto &[app_name, app] : inited_apps)
    {
        std::cout << "App: " << app_name << ", ID: " << app->getId() << std::endl;
    }

    return 0;
}
