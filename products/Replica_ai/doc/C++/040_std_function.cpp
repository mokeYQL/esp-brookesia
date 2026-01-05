/*
 * 知识点：std::function可调用对象包装器
 * 源项目：e:\3.esp-brookesia\esp-brookesia\products\speaker\main\modules\system.cpp 第200-202行
 * C++ Primer中文版（第五版）：第10章 泛型算法 - 10.3 定制操作
 * Effective+C++中文版（第三版）：条款54 让自己熟悉TR1和Boost
 */
#include <iostream>
#include <functional>
#include <vector>
#include <string>
#include <optional>
#include <map>

// 函数定义结构体
struct FunctionDefinition
{
    using Callback = std::function<bool(const std::vector<std::string> &params)>;

    struct CallbackThreadConfig
    {
        std::string name;
        size_t stack_size;
        bool stack_in_ext;
    };

    // 默认构造函数
    FunctionDefinition() = default;

    // 带参数的构造函数
    FunctionDefinition(const std::string &name, const std::string &description)
        : name(name), description(description) {}

    void addParameter(const std::string &param_name, const std::string &param_description)
    {
        parameters.emplace_back(param_name, param_description);
    }

    void setCallback(Callback callback, std::optional<CallbackThreadConfig> thread_config = std::nullopt)
    {
        this->callback = callback;
        this->thread_config = thread_config;
    }

    bool execute(const std::vector<std::string> &params)
    {
        if (callback)
        {
            std::cout << "Executing function: " << name << std::endl;
            return callback(params);
        }
        std::cout << "No callback set for function: " << name << std::endl;
        return false;
    }

    std::string name;
    std::string description;
    std::vector<std::pair<std::string, std::string>> parameters;
    Callback callback;
    std::optional<CallbackThreadConfig> thread_config;
};

// 函数定义列表
class FunctionDefinitionList
{
public:
    static FunctionDefinitionList &requestInstance()
    {
        static FunctionDefinitionList instance;
        return instance;
    }

    bool addFunction(const FunctionDefinition &func_def)
    {
        functions[func_def.name] = func_def;
        std::cout << "Added function: " << func_def.name << std::endl;
        return true;
    }

    bool executeFunction(const std::string &func_name, const std::vector<std::string> &params)
    {
        auto it = functions.find(func_name);
        if (it != functions.end())
        {
            return it->second.execute(params);
        }
        std::cout << "Function not found: " << func_name << std::endl;
        return false;
    }

private:
    FunctionDefinitionList() = default;
    std::map<std::string, FunctionDefinition> functions;
};

// 普通函数
bool openAppFunction(const std::vector<std::string> &params)
{
    for (const auto &param : params)
    {
        std::cout << "Opening app with parameter: " << param << std::endl;
    }
    return true;
}

int main()
{
    // 创建函数定义
    FunctionDefinition openApp("open_app", "Open a specific app.打开一个应用");
    openApp.addParameter("app_name", "The name of the app to open.应用名称");

    // 使用普通函数作为回调
    openApp.setCallback(openAppFunction);

    // 使用lambda作为回调
    FunctionDefinition setVolume("set_volume", "Adjust the system volume. Range is from 0 to 100.");
    setVolume.addParameter("level", "The desired volume level (0 to 100).");

    setVolume.setCallback([](const std::vector<std::string> &params)
                          {
        for (const auto& param : params) {
            std::cout << "Setting volume to: " << param << std::endl;
        }
        return true; });

    // 添加到函数列表
    FunctionDefinitionList::requestInstance().addFunction(openApp);
    FunctionDefinitionList::requestInstance().addFunction(setVolume);

    // 执行函数
    std::cout << "\n=== Executing functions ===" << std::endl;
    FunctionDefinitionList::requestInstance().executeFunction("open_app", {"Calculator"});
    FunctionDefinitionList::requestInstance().executeFunction("set_volume", {"80"});

    return 0;
}
