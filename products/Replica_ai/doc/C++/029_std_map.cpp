/*
 * 知识点：std::map关联容器
 * 源项目：e:\3.esp-brookesia\esp-brookesia\products\speaker\main\modules\system.cpp 第207-215行
 * C++ Primer中文版（第五版）：第11章 关联容器 - 11.1 使用关联容器
 * Effective+C++中文版（第三版）：条款24 若所有参数皆需类型转换，请为此采用non-member函数
 */
#include <iostream>
#include <map>
#include <vector>
#include <string>
#include <algorithm>

int main() {
    // 创建应用名称映射表
    std::map<std::string, std::vector<std::string>> app_name_map = {
        {"Settings",   {"setting", "settings", "设置", "设置应用", "设置app"}},
        {"2048",       {"2048", "game", "游戏", "2048游戏", "2048app"}},
        {"Calculator", {"calculator", "calc", "计算器", "计算器应用", "计算器app"}},
        {"Timer",      {"timer", "时钟", "时钟应用", "时钟app"}}
    };
    
    // 查找应用ID的函数
    auto get_app_id = [&](const std::string& target_name) -> int {
        for (const auto& [app_name, alias_names] : app_name_map) {
            // 检查target_name是否在别名列表中
            if (std::find(alias_names.begin(), alias_names.end(), target_name) != alias_names.end()) {
                std::cout << "Found app: " << app_name << " for alias: " << target_name << std::endl;
                // 简单返回一个假的ID
                return static_cast<int>(app_name.size());
            }
        }
        std::cout << "App not found for alias: " << target_name << std::endl;
        return -1;
    };
    
    // 测试查找功能
    get_app_id("设置");
    get_app_id("calc");
    get_app_id("game");
    get_app_id("unknown");
    
    return 0;
}
