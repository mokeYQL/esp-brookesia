/*
 * 知识点：弃用属性
 * 源项目：e:\3.esp-brookesia\esp-brookesia\core\brookesia_core\esp_brookesia.hpp 第184-195行
 * C++ Primer中文版（第五版）：第19章 特殊工具与技术 - 19.4 运行时类型识别
 * Effective+C++中文版（第三版）：无直接对应章节
 */

#include <iostream>
#include <string>

class BaseApp {
public:
    BaseApp(const std::string& name) : _name(name) {
        std::cout << "BaseApp " << _name << " created" << std::endl;
    }
    
    virtual ~BaseApp() {
        std::cout << "BaseApp " << _name << " destroyed" << std::endl;
    }
    
    // 弃用的成员函数：使用[[deprecated]]属性标记
    [[deprecated("Use getSystemContext() instead")]]
    void* getCore() const {
        std::cout << "Warning: getCore() is deprecated, use getSystemContext() instead" << std::endl;
        return nullptr;
    }
    
    // 推荐使用的新函数
    void* getSystemContext() const {
        std::cout << "getSystemContext() called" << std::endl;
        return nullptr;
    }
    
    // 弃用的成员变量访问器
    [[deprecated("Direct access to appId is deprecated, use getId() instead")]]
    int getAppId() const {
        return _id;
    }
    
    // 推荐使用的访问器
    int getId() const {
        return _id;
    }
    
    // 弃用的静态函数
    [[deprecated("Use the new AppManager::createApp() instead")]]
    static BaseApp* create(const std::string& name) {
        return new BaseApp(name);
    }
    
    // 弃用的枚举类型
    [[deprecated("Use AppStatus enum class instead")]]
    enum AppStatusOld {
        UNINSTALLED,
        INSTALLED,
        RUNNING,
        PAUSED,
        CLOSED
    };
    
    // 推荐使用的枚举类
    enum class AppStatus {
        UNINSTALLED,
        INSTALLED,
        RUNNING,
        PAUSED,
        CLOSED
    };
    
    // 使用弃用枚举的函数
    [[deprecated("Use AppStatus enum class instead")]]
    void setStatusOld(AppStatusOld status) {
        _status_old = status;
        std::cout << "setStatusOld() called with status: " << static_cast<int>(_status_old) << std::endl;
    }
    
    // 使用新枚举类的函数
    void setStatus(AppStatus status) {
        _status = status;
        std::cout << "setStatus() called with status: " << static_cast<int>(_status) << std::endl;
    }
    
private:
    std::string _name;
    int _id = 0;
    AppStatusOld _status_old = AppStatusOld::UNINSTALLED;
    AppStatus _status = AppStatus::UNINSTALLED;
};

// 弃用的全局函数
[[deprecated("Use App::create() instead")]]
BaseApp* createApp(const std::string& name) {
    std::cout << "Warning: createApp() is deprecated, use App::create() instead" << std::endl;
    return new BaseApp(name);
}

// 弃用的全局变量
[[deprecated("Use App::MAX_APPS instead")]]
const int MAX_APPS_OLD = 10;

// 推荐使用的常量
constexpr int MAX_APPS_NEW = 20;

class AppManager {
public:
    // 推荐使用的工厂方法
    static BaseApp* createApp(const std::string& name) {
        std::cout << "AppManager::createApp() called" << std::endl;
        return new BaseApp(name);
    }
};

int main() {
    std::cout << "=== Deprecated Attribute Examples ===" << std::endl;
    
    // 创建应用实例
    BaseApp app("TestApp");
    
    std::cout << "\n1. Deprecated Member Function:" << std::endl;
    // 调用弃用的成员函数
    app.getCore();  // 会产生编译器警告
    
    // 调用推荐的新函数
    app.getSystemContext();
    
    std::cout << "\n2. Deprecated Accessor:" << std::endl;
    // 调用弃用的访问器
    int old_id = app.getAppId();  // 会产生编译器警告
    std::cout << "Old app ID: " << old_id << std::endl;
    
    // 调用推荐的访问器
    int new_id = app.getId();
    std::cout << "New app ID: " << new_id << std::endl;
    
    std::cout << "\n3. Deprecated Static Function:" << std::endl;
    // 调用弃用的静态函数
    BaseApp* app2 = BaseApp::create("DeprecatedApp");  // 会产生编译器警告
    delete app2;
    
    // 调用推荐的工厂方法
    BaseApp* app3 = AppManager::createApp("RecommendedApp");
    delete app3;
    
    std::cout << "\n4. Deprecated Enum vs New Enum Class:" << std::endl;
    // 使用弃用的枚举类型
    app.setStatusOld(BaseApp::AppStatusOld::RUNNING);  // 会产生编译器警告
    
    // 使用推荐的枚举类
    app.setStatus(BaseApp::AppStatus::RUNNING);
    
    std::cout << "\n5. Deprecated Global Function:" << std::endl;
    // 调用弃用的全局函数
    BaseApp* app4 = createApp("GlobalDeprecatedApp");  // 会产生编译器警告
    delete app4;
    
    std::cout << "\n6. Deprecated Global Variable:" << std::endl;
    // 使用弃用的全局变量
    std::cout << "Old max apps: " << MAX_APPS_OLD << std::endl;  // 会产生编译器警告
    
    // 使用推荐的常量
    std::cout << "New max apps: " << MAX_APPS_NEW << std::endl;
    
    return 0;
}