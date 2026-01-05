/*
 * 知识点：std::any类型擦除
 * 源项目：e:\3.esp-brookesia\esp-brookesia\products\speaker\main\modules\system.cpp 第164-167行
 * C++ Primer中文版（第五版）：第16章 模板与泛型编程 - 16.1 定义模板
 * Effective+C++中文版（第三版）：条款31 千万不要返回局部对象的引用，或指向它们的指针
 */
#include <iostream>
#include <any>
#include <string>

class SettingsManager
{
public:
    enum class EventType
    {
        EnterDeveloperMode,
        EnterScreen
    };

    using EventData = std::any;
    using EventDataEnterDeveloperMode = bool;
    using EventDataEnterScreenIndex = int;

    static const int UI_Screen_MORE_ABOUT;
};

// 在类外部定义静态常量
const int SettingsManager::UI_Screen_MORE_ABOUT = 5;

void processEvent(SettingsManager::EventType event_type, const SettingsManager::EventData &event_data)
{
    switch (event_type)
    {
    case SettingsManager::EventType::EnterDeveloperMode:
    {
        // 检查类型并转换
        if (event_data.type() == typeid(SettingsManager::EventDataEnterDeveloperMode))
        {
            auto dev_mode = std::any_cast<SettingsManager::EventDataEnterDeveloperMode>(event_data);
            std::cout << "Enter developer mode: " << (dev_mode ? "true" : "false") << std::endl;
        }
        break;
    }
    case SettingsManager::EventType::EnterScreen:
    {
        // 检查类型并转换
        if (event_data.type() == typeid(SettingsManager::EventDataEnterScreenIndex))
        {
            auto screen_index = std::any_cast<SettingsManager::EventDataEnterScreenIndex>(event_data);
            std::cout << "Enter screen index: " << screen_index << std::endl;

            if (screen_index == SettingsManager::UI_Screen_MORE_ABOUT)
            {
                std::cout << "Updating about info..." << std::endl;
            }
        }
        break;
    }
    default:
        std::cout << "Unknown event type" << std::endl;
    }
}

int main()
{
    // 处理开发者模式事件
    SettingsManager::EventData dev_mode_data = true;
    processEvent(SettingsManager::EventType::EnterDeveloperMode, dev_mode_data);

    // 处理进入屏幕事件
    SettingsManager::EventData screen_data = SettingsManager::UI_Screen_MORE_ABOUT;
    processEvent(SettingsManager::EventType::EnterScreen, screen_data);

    return 0;
}
