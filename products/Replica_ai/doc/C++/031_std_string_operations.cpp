/*
 * 知识点：std::string字符串操作
 * 源项目：e:\3.esp-brookesia\esp-brookesia\products\speaker\main\modules\system.cpp 第456-470行
 * C++ Primer中文版（第五版）：第3章 字符串、向量和数组 - 3.2 标准库类型string
 * Effective+C++中文版（第三版）：条款17 以独立语句将newed对象置入智能指针
 */
#include <iostream>
#include <string>
#include <cctype>

// 将字符串转换为小写
static std::string to_lower(const std::string& input) {
    std::string result = input;
    for (char& c : result) {
        c = std::tolower(static_cast<unsigned char>(c));
    }
    return result;
}

// 获取空格前的字符串
static std::string get_before_space(const std::string& input) {
    size_t pos = input.find(' ');
    return input.substr(0, pos);
}

int main() {
    std::string test_strings[] = {
        "Hello World",
        "OPEN_APP Calculator",
        "SET_VOLUME 80",
        "Settings"
    };
    
    for (const auto& test_str : test_strings) {
        std::cout << "Original: '" << test_str << "'" << std::endl;
        std::cout << "To lower: '" << to_lower(test_str) << "'" << std::endl;
        std::cout << "Before space: '" << get_before_space(test_str) << "'" << std::endl;
        
        // 组合使用
        std::string processed = to_lower(get_before_space(test_str));
        std::cout << "Processed: '" << processed << "'" << std::endl;
        std::cout << "-------------------" << std::endl;
    }
    
    return 0;
}
