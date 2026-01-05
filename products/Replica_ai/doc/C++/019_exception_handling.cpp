/*
 * 知识点：异常处理
 * 源项目：e:\3.esp-brookesia\esp-brookesia\core\brookesia_core\systems\base\esp_brookesia_base_app.cpp 第343-344行
 * C++ Primer中文版（第五版）：第18章 用于大型程序的工具 - 18.1 异常处理
 * Effective+C++中文版（第三版）：条款7 为多态基类声明virtual析构函数
 */

#include <iostream>
#include <string>
#include <stdexcept>
#include <vector>
#include <memory>

// 1. 自定义异常类
class AppException : public std::runtime_error {
public:
    AppException(const std::string& message) : std::runtime_error(message) {
        std::cout << "AppException created: " << message << std::endl;
    }
    
    ~AppException() {
        std::cout << "AppException destroyed" << std::endl;
    }
    
    // 自定义异常类型
    enum ErrorType {
        INIT_FAILED,
        RUN_FAILED,
        RESOURCE_ERROR,
        INVALID_PARAMETER,
        NETWORK_ERROR
    };
    
    AppException(ErrorType type, const std::string& message) 
        : std::runtime_error(message), _type(type) {
        std::cout << "AppException with type created: " << message << std::endl;
    }
    
    ErrorType getType() const {
        return _type;
    }
    
private:
    ErrorType _type;
};

class App {
public:
    App(const std::string& name) : _name(name), _initialized(false) {
        std::cout << "App " << _name << " created" << std::endl;
    }
    
    ~App() {
        std::cout << "App " << _name << " destroyed" << std::endl;
        close();
    }
    
    // 2. 抛出异常的初始化方法
    void initialize() {
        try {
            std::cout << "Initializing app " << _name << "..." << std::endl;
            
            // 模拟初始化过程
            if (_name.empty()) {
                throw std::invalid_argument("App name cannot be empty");
            }
            
            // 模拟资源分配失败
            if (_name == "ErrorApp") {
                throw AppException(AppException::INIT_FAILED, "Failed to allocate resources");
            }
            
            _initialized = true;
            std::cout << "App " << _name << " initialized successfully" << std::endl;
        } catch (const std::exception& e) {
            // 捕获并重新抛出异常
            std::cout << "Caught exception during initialization: " << e.what() << std::endl;
            throw;  // 重新抛出，让调用者处理
        }
    }
    
    // 3. 抛出异常的运行方法
    void run() {
        if (!_initialized) {
            throw AppException(AppException::RUN_FAILED, "App not initialized");
        }
        
        std::cout << "App " << _name << " running" << std::endl;
        
        // 模拟运行时异常
        if (_name == "CrashApp") {
            throw AppException(AppException::RESOURCE_ERROR, "Resource access violation");
        }
    }
    
    // 4. 安全关闭方法
    void close() {
        try {
            if (_initialized) {
                std::cout << "Closing app " << _name << "..." << std::endl;
                _initialized = false;
                // 清理资源
            }
        } catch (const std::exception& e) {
            std::cerr << "Error during close: " << e.what() << std::endl;
            // 关闭过程中的异常不应传播，避免终止程序
        }
    }
    
    // 5. 访问器方法，可能抛出异常
    int getResourceCount() const {
        if (!_initialized) {
            throw std::logic_error("Cannot access resources: app not initialized");
        }
        return _resource_count;
    }
    
    // 6. 设置器方法，可能抛出异常
    void setResourceCount(int count) {
        if (count < 0) {
            throw std::invalid_argument("Resource count cannot be negative");
        }
        if (!_initialized) {
            throw std::logic_error("Cannot modify resources: app not initialized");
        }
        _resource_count = count;
        std::cout << "Resource count set to: " << count << std::endl;
    }
    
    std::string getName() const {
        return _name;
    }
    
    bool isInitialized() const {
        return _initialized;
    }
    
private:
    std::string _name;
    bool _initialized;
    int _resource_count = 0;
};

// 7. 使用智能指针管理资源，自动释放
using AppPtr = std::unique_ptr<App>;

// 8. 异常安全的资源管理
AppPtr createApp(const std::string& name) {
    try {
        auto app = std::make_unique<App>(name);
        app->initialize();
        return app;
    } catch (const std::exception& e) {
        std::cerr << "Failed to create app " << name << ": " << e.what() << std::endl;
        return nullptr;
    }
}

// 9. 主函数中的异常处理
int main() {
    std::cout << "=== Exception Handling Examples ===" << std::endl;
    
    std::cout << "\n1. Try-Catch Block:" << std::endl;
    try {
        App app("TestApp");
        app.initialize();
        app.run();
        std::cout << "App execution completed successfully" << std::endl;
    } catch (const AppException& e) {
        std::cerr << "AppException caught: " << e.what() << ", Type: " << static_cast<int>(e.getType()) << std::endl;
    } catch (const std::exception& e) {
        std::cerr << "Standard exception caught: " << e.what() << std::endl;
    } catch (...) {
        std::cerr << "Unknown exception caught" << std::endl;
    }
    
    std::cout << "\n2. Catching Different Exception Types:" << std::endl;
    try {
        App app("ErrorApp");
        app.initialize();
        app.run();
    } catch (const AppException& e) {
        std::cerr << "App-specific exception: " << e.what() << std::endl;
        if (e.getType() == AppException::INIT_FAILED) {
            std::cerr << "Initialization failed, trying fallback..." << std::endl;
            // 可以在这里添加恢复逻辑
        }
    } catch (const std::invalid_argument& e) {
        std::cerr << "Invalid argument: " << e.what() << std::endl;
    } catch (const std::logic_error& e) {
        std::cerr << "Logic error: " << e.what() << std::endl;
    } catch (const std::runtime_error& e) {
        std::cerr << "Runtime error: " << e.what() << std::endl;
    } catch (const std::exception& e) {
        std::cerr << "Generic exception: " << e.what() << std::endl;
    } catch (...) {
        std::cerr << "Unknown error occurred" << std::endl;
    }
    
    std::cout << "\n3. Smart Pointer and Exception Safety:" << std::endl;
    auto app1 = createApp("SafeApp");
    if (app1) {
        app1->run();
        try {
            app1->setResourceCount(10);
            app1->setResourceCount(-5);  // 这将抛出异常
        } catch (const std::exception& e) {
            std::cerr << "Error setting resource count: " << e.what() << std::endl;
        }
    }
    
    // 智能指针会自动释放资源，即使发生异常
    auto app2 = createApp("CrashApp");
    if (app2) {
        try {
            app2->run();  // 这将抛出异常
        } catch (const AppException& e) {
            std::cerr << "App crashed: " << e.what() << std::endl;
        }
    }
    
    std::cout << "\n4. Vector Access with Exception:" << std::endl;
    std::vector<int> vec = {1, 2, 3, 4, 5};
    try {
        // 安全访问：使用at()方法，越界时抛出异常
        std::cout << "vec[2]: " << vec.at(2) << std::endl;
        std::cout << "vec[10]: " << vec.at(10) << std::endl;  // 越界访问，抛出std::out_of_range
    } catch (const std::out_of_range& e) {
        std::cerr << "Vector out of range: " << e.what() << std::endl;
    }
    
    std::cout << "\n5. Finally-like Behavior with Destructors:" << std::endl;
    try {
        App app("FinallyApp");
        app.initialize();
        app.run();
        // 即使这里发生异常，app的析构函数也会被调用，执行close()方法
        throw AppException(AppException::RUN_FAILED, "Simulated runtime error");
    } catch (const AppException& e) {
        std::cerr << "Caught exception: " << e.what() << std::endl;
        // app的析构函数已经在catch块执行前调用了
    }
    
    std::cout << "\n6. Re-throwing Exceptions:" << std::endl;
    try {
        try {
            App app("ReThrowApp");
            app.initialize();
            throw AppException(AppException::RESOURCE_ERROR, "Inner exception");
        } catch (const AppException& e) {
            std::cerr << "Inner catch: " << e.what() << std::endl;
            // 重新抛出异常，让外层处理
            throw;
        }
    } catch (const AppException& e) {
        std::cerr << "Outer catch: " << e.what() << std::endl;
    }
    
    std::cout << "\n=== Exception Handling Examples Finished ===" << std::endl;
    
    return 0;
}