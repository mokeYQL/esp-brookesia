/*
 * SPDX-FileCopyrightText: 2025 ESP-Brookesia Learning Project
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <cstdio>
#include "esp_brookesia.hpp"
#include "esp_brookesia_systems_internal.h"

using namespace esp_brookesia::systems::base;

// ============================================================
// 第一个学习应用：Hello World
// ============================================================
class HelloWorldApp : public App {
public:
    // 返回应用配置
    const char *getName() const override {
        return "Hello World";
    }

    // 获取应用图标（可选）
    gui::StyleImage getLauncherIcon() const override {
        return gui::StyleImage::IMAGE_NULL();
    }

    // === 生命周期方法 ===

    // 1. init() - 应用安装时调用（只调用一次）
    bool init() override {
        printf("[HelloWorld] init() called\n");
        return true;
    }

    // 2. run() - 应用运行时调用（必须实现）
    //    这是应用的入口点，在这里创建 UI
    bool run() override {
        printf("[HelloWorld] run() called\n");
        printf("[HelloWorld] Creating UI...\n");

        // 获取当前屏幕
        lv_obj_t *screen = getActiveScreen();
        if (!screen) {
            printf("[HelloWorld] Error: No active screen!\n");
            return false;
        }

        // 创建标签
        lv_obj_t *label = lv_label_create(screen);
        if (!label) {
            printf("[HelloWorld] Error: Failed to create label!\n");
            return false;
        }

        // 设置文本
        lv_label_set_text(label, "Hello, ESP-Brookesia!");
        lv_obj_center(label);

        // 设置样式
        lv_obj_set_style_text_font(label, getDefaultFont(), 0);
        lv_obj_set_style_text_color(label, lv_color_white(), 0);

        printf("[HelloWorld] UI created successfully!\n");
        return true;
    }

    // 3. back() - 用户按返回键时调用
    bool back() override {
        printf("[HelloWorld] back() called\n");
        // 返回 false 表示不需要处理，让系统处理
        return false;
    }

    // 4. pause() - 应用暂停时调用
    bool pause() override {
        printf("[HelloWorld] pause() called\n");
        return true;
    }

    // 5. resume() - 应用恢复时调用
    bool resume() override {
        printf("[HelloWorld] resume() called\n");
        return true;
    }

    // 6. close() - 应用关闭时调用
    bool close() override {
        printf("[HelloWorld] close() called\n");
        return true;
    }

    // 7. deinit() - 应用卸载时调用（只调用一次）
    bool deinit() override {
        printf("[HelloWorld] deinit() called\n");
        return true;
    }
};

// ============================================================
// 主函数
// ============================================================
extern "C" void app_main(void)
{
    printf("\n");
    printf("========================================\n");
    printf("   ESP-Brookesia Learning Project    \n");
    printf("   Lesson 1: Hello World App        \n");
    printf("========================================\n\n");

    // 1. 创建 Context（系统上下文）
    // Context 是整个框架的核心，管理所有资源
    Context::Config context_config = {
        .core_data = {
            .screen_size = {
                .width = 360,
                .height = 360,
            },
        },
    };

    Context core;
    if (!core.begin(context_config)) {
        printf("[Main] Error: Failed to initialize Context!\n");
        return;
    }
    printf("[Main] Context initialized successfully\n");

    // 2. 获取 Manager（应用管理器）
    Manager &manager = core.getManager();
    if (!manager.begin()) {
        printf("[Main] Error: Failed to initialize Manager!\n");
        return;
    }
    printf("[Main] Manager initialized successfully\n");

    // 3. 创建 Hello World 应用
    auto hello_world_app = std::make_shared<HelloWorldApp>();

    // 4. 安装应用
    int app_id = manager.installApp(hello_world_app.get());
    if (app_id < 0) {
        printf("[Main] Error: Failed to install app!\n");
        return;
    }
    printf("[Main] App installed successfully (ID: %d)\n", app_id);

    // 5. 启动应用
    if (!manager.startApp(app_id)) {
        printf("[Main] Error: Failed to start app!\n");
        return;
    }
    printf("[Main] App started successfully\n");

    // 6. 主循环
    printf("\n[Main] Application running...\n");
    printf("[Main] Press Ctrl+C to stop\n\n");

    while (true) {
        vTaskDelay(pdMS_TO_TICKS(1000));
    }
}
