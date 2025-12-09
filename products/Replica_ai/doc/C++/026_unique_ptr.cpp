/*
 * 知识点：std::unique_ptr智能指针
 * 源项目：e:\3.esp-brookesia\esp-brookesia\products\speaker\main\modules\system.cpp 第102-109行
 * C++ Primer中文版（第五版）：第12章 动态内存 - 12.1.1 unique_ptr类
 * Effective+C++中文版（第三版）：条款18 让接口容易被正确使用，不易被误用
 */
#include <iostream>
#include <memory>
#include <string>

class Stylesheet {
public:
    struct CoreInfo {
        std::string name;
    };
    CoreInfo core;
    
    Stylesheet(const std::string& name) {
        core.name = name;
        std::cout << "Stylesheet " << core.name << " created" << std::endl;
    }
    
    ~Stylesheet() {
        std::cout << "Stylesheet " << core.name << " destroyed" << std::endl;
    }
};

class Speaker {
public:
    bool addStylesheet(const Stylesheet* stylesheet) {
        std::cout << "Added stylesheet: " << stylesheet->core.name << std::endl;
        return true;
    }
    
    bool activateStylesheet(const Stylesheet* stylesheet) {
        std::cout << "Activated stylesheet: " << stylesheet->core.name << std::endl;
        return true;
    }
};

int main() {
    Speaker speaker;
    
    // 使用std::make_unique创建unique_ptr
    std::unique_ptr<Stylesheet> stylesheet = std::make_unique<Stylesheet>("dark_theme");
    
    // 传递原始指针给函数
    speaker.addStylesheet(stylesheet.get());
    speaker.activateStylesheet(stylesheet.get());
    
    // 重置unique_ptr，释放资源
    stylesheet = nullptr;
    
    std::cout << "Program ended" << std::endl;
    return 0;
}
