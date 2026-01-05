/*
 * 知识点：外部C链接
 * 源项目：e:\3.esp-brookesia\esp-brookesia\products\speaker\main\modules\touch_sensor.cpp 第18-20行
 * C++ Primer中文版（第五版）：第12章 动态内存 - 12.1.5 智能指针和异常
 * Effective+C++中文版（第三版）：无直接对应章节
 */

#include <iostream>
#include <string>

// 1. 外部C函数声明：在C++代码中调用C函数
// 假设这是一个C库中的函数
#ifdef __cplusplus
extern "C" {
#endif

void c_function(int value);
const char* c_get_version();
void c_callback_register(void (*callback)(int));

#ifdef __cplusplus
}
#endif

// 2. C++函数暴露给C代码：在C代码中调用C++函数
// 这些函数可以被C代码调用
#ifdef __cplusplus
extern "C" {
#endif

void cpp_function(int value);
const char* cpp_get_app_name();
int cpp_add(int a, int b);

#ifdef __cplusplus
}
#endif

// 3. 全局变量的外部C链接
#ifdef __cplusplus
extern "C" {
#endif

// 声明一个C风格的全局变量
extern int g_c_global_var;

#ifdef __cplusplus
}
#endif

// 4. C++回调函数暴露给C代码
static void cpp_callback(int value) {
    std::cout << "C++ callback called with value: " << value << std::endl;
}

// 5. 包装C++类供C代码使用
class App {
public:
    App(const std::string& name) : _name(name) {
        std::cout << "App " << _name << " created" << std::endl;
    }
    
    ~App() {
        std::cout << "App " << _name << " destroyed" << std::endl;
    }
    
    void run() {
        std::cout << "App " << _name << " running" << std::endl;
    }
    
    std::string getName() const {
        return _name;
    }
    
private:
    std::string _name;
};

// 包装函数：C接口创建App对象
#ifdef __cplusplus
extern "C" {

void* app_create(const char* name) {
    return new App(name);
}

void app_run(void* app) {
    if (app) {
        static_cast<App*>(app)->run();
    }
}

const char* app_get_name(void* app) {
    if (app) {
        // 注意：返回的指针指向临时对象，实际使用中应使用静态缓冲区或动态分配
        static std::string name;
        name = static_cast<App*>(app)->getName();
        return name.c_str();
    }
    return nullptr;
}

void app_destroy(void* app) {
    if (app) {
        delete static_cast<App*>(app);
    }
}

}
#endif

// 6. 实现暴露给C的C++函数
void cpp_function(int value) {
    std::cout << "C++ function called with value: " << value << std::endl;
}

const char* cpp_get_app_name() {
    static std::string app_name = "ESP-Brookesia";
    return app_name.c_str();
}

int cpp_add(int a, int b) {
    return a + b;
}

// 7. 模拟C函数的实现（实际中这些函数会在C库中实现）
#ifdef __cplusplus
extern "C" {

void c_function(int value) {
    std::cout << "C function called with value: " << value << std::endl;
}

const char* c_get_version() {
    return "1.0.0";
}

void c_callback_register(void (*callback)(int)) {
    std::cout << "C callback registered" << std::endl;
    // 模拟调用回调函数
    callback(42);
}

int g_c_global_var = 100;

}
#endif

int main() {
    std::cout << "=== Extern C Examples ===" << std::endl;
    
    std::cout << "\n1. Calling C functions from C++:" << std::endl;
    c_function(10);
    
    const char* version = c_get_version();
    std::cout << "C library version: " << version << std::endl;
    
    std::cout << "\n2. Calling C++ functions from C (simulated):" << std::endl;
    cpp_function(20);
    
    const char* app_name = cpp_get_app_name();
    std::cout << "C++ app name: " << app_name << std::endl;
    
    int sum = cpp_add(5, 7);
    std::cout << "C++ add result: " << sum << std::endl;
    
    std::cout << "\n3. Using C global variables in C++:" << std::endl;
    std::cout << "C global variable value: " << g_c_global_var << std::endl;
    g_c_global_var = 200;
    std::cout << "C global variable after modification: " << g_c_global_var << std::endl;
    
    std::cout << "\n4. Registering C++ callback with C library:" << std::endl;
    c_callback_register(cpp_callback);
    
    std::cout << "\n5. Using C wrapper for C++ class:" << std::endl;
    // 使用C风格的API创建和使用App对象
    void* c_app = app_create("TestApp");
    if (c_app) {
        app_run(c_app);
        
        const char* name = app_get_name(c_app);
        std::cout << "App name from C API: " << name << std::endl;
        
        app_destroy(c_app);
    }
    
    std::cout << "\n=== Extern C Examples Finished ===" << std::endl;
    
    return 0;
}