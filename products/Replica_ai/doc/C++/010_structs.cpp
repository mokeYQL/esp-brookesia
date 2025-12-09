/*
 * 知识点：结构体
 * 源项目：e:\3.esp-brookesia\esp-brookesia\core\brookesia_core\systems\base\esp_brookesia_base_app.hpp 第28-76行
 * C++ Primer中文版（第五版）：第2章 变量和基本类型 - 2.6 自定义数据结构
 * Effective+C++中文版（第三版）：无直接对应章节
 */

#include <iostream>
#include <string>
#include <cstdint>

// 基础结构体：配置数据
struct AppConfig {
    std::string name;               // 应用名称
    const void* launcher_icon;      // 启动图标
    uint16_t screen_width;          // 屏幕宽度
    uint16_t screen_height;         // 屏幕高度
    
    // 结构体嵌套
    struct {
        bool enable_default_screen : 1;       // 位域：是否启用默认屏幕
        bool enable_recycle_resource : 1;     // 位域：是否启用资源回收
        bool enable_resize_visual_area : 1;   // 位域：是否启用可视区域调整
    } flags;                                  // 配置标志
    
    // 静态成员函数：结构体构造辅助函数
    static constexpr AppConfig create(const char* app_name, const void* icon, bool use_default_screen) {
        return {
            .name = app_name,
            .launcher_icon = icon,
            .screen_width = 320,
            .screen_height = 240,
            .flags = {
                .enable_default_screen = use_default_screen,
                .enable_recycle_resource = true,
                .enable_resize_visual_area = true
            }
        };
    }
};

// 另一个结构体：显示数据
struct DisplayData {
    int width;      // 显示宽度
    int height;     // 显示高度
    int x_offset;   // X偏移
    int y_offset;   // Y偏移
    
    // 成员函数：计算显示区域面积
    int getArea() const {
        return width * height;
    }
    
    // 成员函数：检查是否有效
    bool isValid() const {
        return (width > 0) && (height > 0);
    }
};

// 结构体数组
struct Point {
    int x;
    int y;
    
    // 构造函数（C++11及以上）
    Point(int x_val, int y_val) : x(x_val), y(y_val) {}
    
    // 默认构造函数
    Point() : x(0), y(0) {}
};

// 使用结构体的类
class App {
public:
    // 使用结构体初始化
    App(const AppConfig& config) : _config(config) {
        std::cout << "App " << _config.name << " created" << std::endl;
    }
    
    ~App() {
        std::cout << "App " << _config.name << " destroyed" << std::endl;
    }
    
    // 使用结构体作为函数参数
    void updateDisplay(const DisplayData& display) {
        _display = display;
        std::cout << "Display updated: " << _display.width << "x" << _display.height << std::endl;
        std::cout << "Display area: " << _display.getArea() << std::endl;
    }
    
    // 使用结构体作为返回值
    DisplayData getDisplay() const {
        return _display;
    }
    
    // 结构体成员访问
    bool isDefaultScreenEnabled() const {
        return _config.flags.enable_default_screen;
    }
    
private:
    AppConfig _config;      // 结构体作为成员变量
    DisplayData _display;   // 结构体作为成员变量
};

// 结构体指针和引用
void modifyStructByReference(DisplayData& display) {
    display.width += 10;
    display.height += 10;
}

void modifyStructByPointer(DisplayData* display) {
    if (display != nullptr) {
        display->width += 20;
        display->height += 20;
    }
}

// 结构体数组使用
void useStructArray() {
    std::cout << "\n=== Struct Array Usage ===" << std::endl;
    
    // 静态结构体数组
    Point points[5] = {
        Point(0, 0),
        Point(10, 20),
        Point(20, 40),
        Point(30, 60),
        Point(40, 80)
    };
    
    // 遍历结构体数组
    for (int i = 0; i < 5; i++) {
        std::cout << "Point " << i << ": (" << points[i].x << ", " << points[i].y << ")" << std::endl;
    }
}

int main() {
    std::cout << "=== Struct Usage Examples ===" << std::endl;
    
    // 1. 使用静态成员函数创建结构体实例
    AppConfig config = AppConfig::create("Calculator", nullptr, true);
    
    // 2. 创建应用实例
    App app(config);
    
    // 3. 使用结构体
    DisplayData display = {320, 240, 0, 0};
    app.updateDisplay(display);
    
    // 4. 检查结构体有效性
    if (display.isValid()) {
        std::cout << "Display is valid" << std::endl;
    }
    
    // 5. 通过引用修改结构体
    modifyStructByReference(display);
    app.updateDisplay(display);
    
    // 6. 通过指针修改结构体
    modifyStructByPointer(&display);
    app.updateDisplay(display);
    
    // 7. 访问结构体成员
    std::cout << "Default screen enabled: " << (app.isDefaultScreenEnabled() ? "true" : "false") << std::endl;
    
    // 8. 结构体数组使用
    useStructArray();
    
    return 0;
}