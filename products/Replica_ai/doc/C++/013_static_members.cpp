/*
 * 知识点：静态成员
 * 源项目：e:\3.esp-brookesia\esp-brookesia\core\brookesia_core\systems\base\esp_brookesia_base_app.hpp 第87-88行
 * C++ Primer中文版（第五版）：第7章 类 - 7.6 类的其他特性 - 7.6.2 静态成员
 * Effective+C++中文版（第三版）：条款4 确定对象被使用前已先被初始化
 */

#include <iostream>
#include <string>
#include <vector>
#include <map>

class App
{
public:
    // 静态常量：类级别的常量
    static constexpr int APP_ID_MIN = 1;
    static constexpr int APP_ID_MAX = 100;

    // 静态变量：类级别的变量，所有实例共享
    static int appCount;
    static std::vector<std::string> appNames;

    // 构造函数
    App(const std::string &name) : _name(name)
    {
        // 初始化实例成员
        _id = ++appCount;         // 使用静态变量
        appNames.push_back(name); // 使用静态容器
        std::cout << "App " << _name << " created, ID=" << _id << ", Total apps=" << appCount << std::endl;
    }

    ~App()
    {
        std::cout << "App " << _name << " destroyed" << std::endl;
    }

    // 静态成员函数：类级别的函数，不访问实例成员
    static int getAppCount()
    {
        return appCount;
    }

    static std::vector<std::string> getAllAppNames()
    {
        return appNames;
    }

    static bool isValidAppId(int id)
    {
        return (id >= APP_ID_MIN) && (id <= APP_ID_MAX);
    }

    // 实例成员函数
    int getId() const
    {
        return _id;
    }

    std::string getName() const
    {
        return _name;
    }

private:
    // 实例成员变量
    int _id;
    std::string _name;
};

// 静态成员变量的初始化（必须在类外部进行）
int App::appCount = 0;
std::vector<std::string> App::appNames = {};

class AppManager
{
public:
    // 静态成员函数：工厂方法
    static App *createApp(const std::string &name)
    {
        if (App::isValidAppId(App::appCount + 1))
        {
            return new App(name);
        }
        else
        {
            std::cout << "Error: Maximum app count reached" << std::endl;
            return nullptr;
        }
    }

    // 静态成员函数：管理函数
    static void showAllApps()
    {
        std::cout << "\n=== All Apps (" << App::getAppCount() << ") ===" << std::endl;
        auto names = App::getAllAppNames();
        for (size_t i = 0; i < names.size(); i++)
        {
            std::cout << "  " << (i + 1) << ". " << names[i] << std::endl;
        }
    }
};

// 单例模式示例：使用静态成员
class SystemConfig
{
public:
    // 禁止拷贝和移动
    SystemConfig(const SystemConfig &) = delete;
    SystemConfig &operator=(const SystemConfig &) = delete;
    SystemConfig(SystemConfig &&) = delete;
    SystemConfig &operator=(SystemConfig &&) = delete;

    // 静态成员函数：获取单例实例
    static SystemConfig &getInstance()
    {
        // 静态局部变量：线程安全的单例初始化（C++11及以上）
        static SystemConfig instance;
        return instance;
    }

    // 设置和获取配置
    void setConfig(const std::string &key, const std::string &value)
    {
        _config[key] = value;
    }

    std::string getConfig(const std::string &key) const
    {
        auto it = _config.find(key);
        if (it != _config.end())
        {
            return it->second;
        }
        return "";
    }

    void showConfig() const
    {
        std::cout << "\n=== System Config ===" << std::endl;
        for (const auto &pair : _config)
        {
            std::cout << "  " << pair.first << ": " << pair.second << std::endl;
        }
    }

private:
    // 私有构造函数：防止外部实例化
    SystemConfig()
    {
        std::cout << "SystemConfig instance created" << std::endl;
        // 默认配置
        _config["version"] = "1.0.0";
        _config["language"] = "en";
        _config["theme"] = "dark";
    }

    // 实例成员变量
    std::map<std::string, std::string> _config;
};

int main()
{
    std::cout << "=== Static Members Examples ===" << std::endl;

    std::cout << "\n1. Static Constants:" << std::endl;
    std::cout << "APP_ID_MIN: " << App::APP_ID_MIN << std::endl;
    std::cout << "APP_ID_MAX: " << App::APP_ID_MAX << std::endl;
    std::cout << "Is 50 a valid app ID? " << (App::isValidAppId(50) ? "true" : "false") << std::endl;
    std::cout << "Is 150 a valid app ID? " << (App::isValidAppId(150) ? "true" : "false") << std::endl;

    std::cout << "\n2. Static Variables and Member Functions:" << std::endl;
    std::cout << "Initial app count: " << App::getAppCount() << std::endl;

    // 创建几个应用实例
    App app1("Calculator");
    App app2("Game 2048");
    App app3("Settings");

    std::cout << "Current app count: " << App::getAppCount() << std::endl;

    // 使用AppManager的静态函数
    AppManager::showAllApps();

    std::cout << "\n3. Static Member Function as Factory Method:" << std::endl;
    // 使用静态工厂方法创建应用
    App *app4 = AppManager::createApp("Browser");
    App *app5 = AppManager::createApp("Music Player");

    if (app4 != nullptr)
    {
        std::cout << "Created app via factory: " << app4->getName() << std::endl;
        delete app4;
    }

    if (app5 != nullptr)
    {
        std::cout << "Created app via factory: " << app5->getName() << std::endl;
        delete app5;
    }

    AppManager::showAllApps();

    std::cout << "\n4. Singleton Pattern with Static Members:" << std::endl;
    // 获取单例实例
    SystemConfig &config1 = SystemConfig::getInstance();
    SystemConfig &config2 = SystemConfig::getInstance();

    // 验证是同一个实例
    std::cout << "Are config1 and config2 the same instance? " << (&config1 == &config2 ? "true" : "false") << std::endl;

    // 使用单例实例
    config1.setConfig("screen_width", "320");
    config1.setConfig("screen_height", "240");

    std::string version = config2.getConfig("version");
    std::cout << "System version: " << version << std::endl;

    config2.showConfig();

    std::cout << "\n5. Final App Count:" << std::endl;
    std::cout << "Total apps: " << App::getAppCount() << std::endl;

    return 0;
}