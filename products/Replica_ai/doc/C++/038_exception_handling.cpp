/*
 * 知识点：异常处理机制
 * 源项目：e:\3.esp-brookesia\esp-brookesia\products\speaker\main\modules\system.cpp 第85-86行
 * C++ Primer中文版（第五版）：第18章 用于大型程序的工具 - 18.1 异常处理
 * Effective+C++中文版（第三版）：条款7 为多态基类声明virtual析构函数
 */
#include <iostream>
#include <stdexcept>
#include <string>
#include <memory>

class Speaker {
public:
    Speaker() {
        std::cout << "Speaker created" << std::endl;
    }
    
    ~Speaker() {
        std::cout << "Speaker destroyed" << std::endl;
    }
    
    void initialize() {
        // 模拟初始化失败
        bool init_success = false;
        if (!init_success) {
            throw std::runtime_error("Failed to initialize speaker");
        }
        std::cout << "Speaker initialized" << std::endl;
    }
    
    void playSound(const std::string& sound_file) {
        if (sound_file.empty()) {
            throw std::invalid_argument("Sound file path cannot be empty");
        }
        std::cout << "Playing sound: " << sound_file << std::endl;
    }
};

// 模拟ESP_UTILS_CHECK_EXCEPTION_RETURN宏的功能
#define CHECK_EXCEPTION_RETURN(expr, ret_val, msg) \
    do { \
        try { \
            expr; \
        } catch (const std::exception& e) { \
            std::cerr << "Error: " << msg << ": " << e.what() << std::endl; \
            return ret_val; \
        } \
    } while (false)

bool createAndInitializeSpeaker() {
    Speaker* speaker = nullptr;
    
    // 使用宏检查异常
    CHECK_EXCEPTION_RETURN(
        speaker = new Speaker(), false, "Create speaker failed"
    );
    
    try {
        speaker->initialize();
        speaker->playSound("test.mp3");
        delete speaker;
        return true;
    } catch (const std::exception& e) {
        std::cerr << "Exception caught: " << e.what() << std::endl;
        delete speaker;
        return false;
    }
}

bool createAndInitializeSpeakerWithSmartPtr() {
    try {
        // 使用智能指针管理内存，自动释放
        auto speaker = std::make_unique<Speaker>();
        speaker->initialize();
        speaker->playSound("test.mp3");
        return true;
    } catch (const std::invalid_argument& e) {
        std::cerr << "Invalid argument: " << e.what() << std::endl;
        return false;
    } catch (const std::runtime_error& e) {
        std::cerr << "Runtime error: " << e.what() << std::endl;
        return false;
    } catch (const std::exception& e) {
        std::cerr << "Generic exception: " << e.what() << std::endl;
        return false;
    }
}

int main() {
    std::cout << "=== Testing exception handling with raw pointer ===" << std::endl;
    createAndInitializeSpeaker();
    
    std::cout << "\n=== Testing exception handling with smart pointer ===" << std::endl;
    createAndInitializeSpeakerWithSmartPtr();
    
    return 0;
}
