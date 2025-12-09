/*
 * 知识点：STL容器
 * 源项目：e:\3.esp-brookesia\esp-brookesia\core\brookesia_core\systems\base\esp_brookesia_base_app.hpp 第465-471行
 * C++ Primer中文版（第五版）：第9章 顺序容器 - 9.1 顺序容器概述
 * Effective+C++中文版（第三版）：条款18 让接口容易被正确使用，不易被误用
 */

#include <iostream>
#include <list>
#include <map>
#include <vector>
#include <string>

// 模拟LVGL对象
class LvObj {
public:
    LvObj(int id) : _id(id) {
        std::cout << "LvObj " << _id << " created" << std::endl;
    }
    
    ~LvObj() {
        std::cout << "LvObj " << _id << " destroyed" << std::endl;
    }
    
    int getId() const {
        return _id;
    }
    
private:
    int _id;
};

// 模拟LVGL定时器
class LvTimer {
public:
    LvTimer(int id, void (*cb)()) : _id(id), _callback(cb) {
        std::cout << "LvTimer " << _id << " created" << std::endl;
    }
    
    ~LvTimer() {
        std::cout << "LvTimer " << _id << " destroyed" << std::endl;
    }
    
    int getId() const {
        return _id;
    }
    
private:
    int _id;
    void (*_callback)();
};

// 模拟LVGL动画
class LvAnim {
public:
    LvAnim(int id) : _id(id) {
        std::cout << "LvAnim " << _id << " created" << std::endl;
    }
    
    ~LvAnim() {
        std::cout << "LvAnim " << _id << " destroyed" << std::endl;
    }
    
    int getId() const {
        return _id;
    }
    
private:
    int _id;
};

class AppResourceManager {
public:
    AppResourceManager() {
        std::cout << "AppResourceManager created" << std::endl;
    }
    
    ~AppResourceManager() {
        std::cout << "AppResourceManager destroyed" << std::endl;
        clearAllResources();
    }
    
    // 添加资源到容器
    void addScreen(LvObj* screen) {
        _screens.push_back(screen);
    }
    
    void addTimer(LvTimer* timer) {
        _timers.push_back(timer);
    }
    
    void addAnimation(LvAnim* anim) {
        _animations.push_back(anim);
    }
    
    // 使用map存储资源映射
    void addScreenMapping(LvObj* screen, const std::string& name) {
        _screenMap[screen] = name;
    }
    
    // 清理所有资源
    void clearAllResources() {
        // 清理list中的资源
        for (auto screen : _screens) {
            delete screen;
        }
        _screens.clear();
        
        for (auto timer : _timers) {
            delete timer;
        }
        _timers.clear();
        
        for (auto anim : _animations) {
            delete anim;
        }
        _animations.clear();
        
        // map自动清理
        _screenMap.clear();
        
        std::cout << "All resources cleared" << std::endl;
    }
    
    // 显示资源统计
    void showResourceStats() {
        std::cout << "Resource Statistics:" << std::endl;
        std::cout << "  Screens: " << _screens.size() << std::endl;
        std::cout << "  Timers: " << _timers.size() << std::endl;
        std::cout << "  Animations: " << _animations.size() << std::endl;
        std::cout << "  Screen Mappings: " << _screenMap.size() << std::endl;
    }
    
private:
    // 使用list存储资源
    std::list<LvObj*> _screens;
    std::list<LvTimer*> _timers;
    std::list<LvAnim*> _animations;
    
    // 使用map存储资源映射
    std::map<LvObj*, std::string> _screenMap;
};

int main() {
    AppResourceManager manager;
    
    // 创建资源
    LvObj* screen1 = new LvObj(1);
    LvObj* screen2 = new LvObj(2);
    LvTimer* timer1 = new LvTimer(1, nullptr);
    LvAnim* anim1 = new LvAnim(1);
    LvAnim* anim2 = new LvAnim(2);
    
    // 添加到资源管理器
    manager.addScreen(screen1);
    manager.addScreen(screen2);
    manager.addTimer(timer1);
    manager.addAnimation(anim1);
    manager.addAnimation(anim2);
    
    // 添加映射
    manager.addScreenMapping(screen1, "Main Screen");
    manager.addScreenMapping(screen2, "Settings Screen");
    
    // 显示统计
    manager.showResourceStats();
    
    // 资源自动清理（在manager析构时）
    
    return 0;
}