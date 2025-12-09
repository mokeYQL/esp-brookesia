/*
 * 知识点：单例模式
 * 源项目：e:\3.esp-brookesia\esp-brookesia\products\speaker\main\modules\system.cpp 第267行
 * C++ Primer中文版（第五版）：第7章 类 - 7.6 类的其他特性 - 7.6.2 静态成员
 * Effective+C++中文版（第三版）：条款04 确定对象被使用前已先被初始化
 */
#include <iostream>
#include <mutex>
#include <string>
#include <vector>

// 函数定义结构体
struct FunctionDefinition {
    std::string name;
    std::string description;
    
    FunctionDefinition(const std::string& name, const std::string& description) 
        : name(name), description(description) {}
};

// 单例模式：函数定义列表
class FunctionDefinitionList {
public:
    // 获取单例实例的静态方法
    static FunctionDefinitionList& requestInstance() {
        // 静态局部变量实现单例（线程安全，C++11及以上）
        static FunctionDefinitionList instance;
        return instance;
    }
    
    // 禁止拷贝构造
    FunctionDefinitionList(const FunctionDefinitionList&) = delete;
    // 禁止拷贝赋值
    FunctionDefinitionList& operator=(const FunctionDefinitionList&) = delete;
    // 禁止移动构造
    FunctionDefinitionList(FunctionDefinitionList&&) = delete;
    // 禁止移动赋值
    FunctionDefinitionList& operator=(FunctionDefinitionList&&) = delete;
    
    // 添加函数
    bool addFunction(const FunctionDefinition& func_def) {
        std::lock_guard<std::mutex> lock(mutex);
        functions.push_back(func_def);
        std::cout << "Added function: " << func_def.name << std::endl;
        return true;
    }
    
    // 获取函数数量
    size_t getFunctionCount() const {
        std::lock_guard<std::mutex> lock(mutex);
        return functions.size();
    }
    
    // 打印所有函数
    void printAllFunctions() const {
        std::lock_guard<std::mutex> lock(mutex);
        std::cout << "=== Available Functions ===" << std::endl;
        for (const auto& func : functions) {
            std::cout << "- " << func.name << ": " << func.description << std::endl;
        }
        std::cout << "=========================" << std::endl;
    }
    
private:
    // 私有构造函数
    FunctionDefinitionList() {
        std::cout << "FunctionDefinitionList instance created" << std::endl;
    }
    
    // 私有析构函数
    ~FunctionDefinitionList() {
        std::cout << "FunctionDefinitionList instance destroyed" << std::endl;
    }
    
    mutable std::mutex mutex; // mutable允许const方法修改
    std::vector<FunctionDefinition> functions;
};

// 测试单例模式
void testSingleton() {
    // 获取单例实例
    FunctionDefinitionList& func_list = FunctionDefinitionList::requestInstance();
    
    // 添加函数
    func_list.addFunction(FunctionDefinition("open_app", "Open a specific app"));
    func_list.addFunction(FunctionDefinition("set_volume", "Adjust system volume"));
    func_list.addFunction(FunctionDefinition("set_brightness", "Adjust display brightness"));
    
    // 打印函数数量
    std::cout << "Function count: " << func_list.getFunctionCount() << std::endl;
    
    // 打印所有函数
    func_list.printAllFunctions();
    
    // 再次获取实例，验证是同一个实例
    FunctionDefinitionList& another_func_list = FunctionDefinitionList::requestInstance();
    std::cout << "Same instance? " << (&func_list == &another_func_list ? "Yes" : "No") << std::endl;
    
    // 再添加一个函数，验证两个引用操作的是同一个实例
    another_func_list.addFunction(FunctionDefinition("restart", "Restart the device"));
    std::cout << "Function count after adding via second reference: " << func_list.getFunctionCount() << std::endl;
    
    func_list.printAllFunctions();
}

int main() {
    std::cout << "=== Testing Singleton Pattern ===" << std::endl;
    testSingleton();
    
    // 程序结束时，单例实例会自动销毁
    std::cout << "\nProgram ending..." << std::endl;
    
    return 0;
}
