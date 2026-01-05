/*
 * SPDX-FileCopyrightText: 2025 ESP-Brookesia Learning Project
 *
 * SPDX-License-Identifier: Apache-2.0
 */

/**
 * @file component_interfaces_demo.cpp
 * @brief ESP-Brookesia 组件接口使用示例
 *
 * 本示例展示了如何使用各个组件提供的接口：
 * 1. Context - 系统上下文
 * 2. Manager - 应用管理器
 * 3. App - 应用基类
 * 4. Display - 显示管理
 * 5. Event - 事件系统
 * 6. GUI - 图形界面
 * 7. StorageNVS - 存储服务
 */

#include <cstdio>
#include <memory>
#include "esp_brookesia.hpp"
#include "esp_brookesia_systems_internal.h"
#include "esp_brookesia_service_storage_nvs.hpp"

using namespace esp_brookesia::systems::base;
using namespace esp_brookesia::gui;
using namespace esp_brookesia::service;

// ============================================================
// 示例应用：组件接口演示
// ============================================================
class ComponentDemoApp : public App {
private:
    std::shared_ptr<LvTimer> _timer;
    int _counter = 0;

public:
    const char *getName() const override {
        return "Component Demo";
    }

    gui::StyleImage getLauncherIcon() const override {
        return gui::StyleImage::IMAGE_NULL();
    }

    // ============================================================
    // 应用生命周期演示
    // ============================================================
    bool init() override {
        printf("\n");
        printf("========================================\n");
        printf("   组件接口演示 - init()         \n");
        printf("========================================\n\n");

        printf("[App] 应用初始化...\n");
        printf("[App]   应用名称: %s\n", getName());
        printf("[App]   应用 ID: %d\n", getId());

        return true;
    }

    bool run() override {
        printf("\n========================================\n");
        printf("   组件接口演示 - run()          \n");
        printf("========================================\n\n");

        lv_obj_t *screen = getActiveScreen();
        if (!screen) {
            printf("[App] 错误: 无法获取屏幕\n");
            return false;
        }

        // ============================================================
        // 1. GUI 组件接口演示
        // ============================================================
        printf("\n--- 1. GUI 组件接口演示 ---\n");

        // 1.1 StyleSize 样式演示
        printf("\n[StyleSize] 演示:\n");

        StyleSize btn_size = StyleSize::RECT(120, 50);
        printf("  - 创建矩形按钮: %dx%d\n", btn_size.width, btn_size.height);

        StyleSize screen_pct = StyleSize::RECT_PERCENT(80, 60);
        printf("  - 创建百分比尺寸: %d%% x %d%%\n", screen_pct.width_percent, screen_pct.height_percent);

        StyleSize circle = StyleSize::CIRCLE(80);
        printf("  - 创建圆形: 直径=%d\n", circle.width);

        // 1.2 StyleColor 样式演示
        printf("\n[StyleColor] 演示:\n");

        StyleColor primary_color = StyleColor::COLOR(0x00AAFF);
        printf("  - 主色: 0x%06X\n", primary_color.color);

        StyleColor text_color = StyleColor::COLOR_WITH_OPACITY(0xFFFFFF, 200);
        printf("  - 文本色: 0x%06X, 透明度: %d\n", text_color.color, text_color.opacity);

        // 1.3 StyleFont 样式演示
        printf("\n[StyleFont] 演示:\n");

        StyleFont title_font = StyleFont::SIZE(20);
        printf("  - 标题字体大小: %dpx\n", title_font.size_px);

        StyleFont body_font = StyleFont::SIZE(16);
        printf("  - 正文字体大小: %dpx\n", body_font.size_px);

        // 1.4 StyleAlign 样式演示
        printf("\n[StyleAlign] 演示:\n");

        StyleAlign center_align = {
            .type = STYLE_ALIGN_TYPE_CENTER,
            .offset_x = 0,
            .offset_y = 0,
        };
        printf("  - 居中对齐: 偏移=(%d, %d)\n", center_align.offset_x, center_align.offset_y);

        // 1.5 StyleGap 样式演示
        printf("\n[StyleGap] 演示:\n");

        StyleGap gap = StyleGap::AROUND(10, 10, 10, 10);
        printf("  - 间距: 上=%d, 下=%d, 左=%d, 右=%d\n", gap.top, gap.bottom, gap.left, gap.right);

        // ============================================================
        // 2. 创建 UI 对象
        // ============================================================
        printf("\n--- 2. 创建 UI 对象 ---\n");

        // 2.1 创建标签
        lv_obj_t *title_label = lv_label_create(screen);
        lv_label_set_text(title_label, "组件接口演示");
        lv_obj_set_style_text_font(title_label, title_font.font_resource, 0);
        lv_obj_set_style_text_color(title_label, lv_color_hex(text_color.color), 0);
        lv_obj_align(title_label, LV_ALIGN_TOP_MID, 0, 20);
        printf("[UI] 创建标题标签\n");

        // 2.2 创建按钮
        lv_obj_t *button = lv_button_create(screen);
        lv_obj_set_size(button, btn_size.width, btn_size.height);
        lv_obj_align(button, LV_ALIGN_CENTER, 0, -20);

        // 按钮标签
        lv_obj_t *btn_label = lv_label_create(button);
        lv_label_set_text(btn_label, "点击计数: 0");
        lv_obj_set_style_text_font(btn_label, body_font.font_resource, 0);
        lv_obj_center(btn_label);
        printf("[UI] 创建按钮\n");

        // 2.3 创建信息标签
        lv_obj_t *info_label = lv_label_create(screen);
        lv_label_set_text(info_label, "等待点击...");
        lv_obj_set_style_text_font(info_label, body_font.font_resource, 0);
        lv_obj_align(info_label, LV_ALIGN_BOTTOM_MID, 0, -20);
        printf("[UI] 创建信息标签\n");

        // 2.4 添加按钮点击事件
        lv_obj_add_event_cb(button, [](lv_event_t *e) {
            lv_obj_t *btn_label = lv_obj_get_child(lv_event_get_target(e), 0);
            _counter++;
            char buf[32];
            snprintf(buf, sizeof(buf), "点击计数: %d", _counter);
            lv_label_set_text(btn_label, buf);
            printf("[Event] 按钮被点击! 计数: %d\n", _counter);
        }, LV_EVENT_CLICKED, nullptr);

        // ============================================================
        // 3. LvTimer 演示
        // ============================================================
        printf("\n--- 3. LvTimer 演示 ---\n");

        _timer = LvHelper::createTimer([this, info_label](void *user_data) {
            _counter++;
            char buf[32];
            snprintf(buf, sizeof(buf), "计时器计数: %d", _counter);
            lv_label_set_text(info_label, buf);
            printf("[Timer] 定时器触发! 计数: %d\n", _counter);
        }, 1000, nullptr);
        _timer->begin();
        printf("[Timer] 创建定时器: 1000ms\n");

        // ============================================================
        // 4. StorageNVS 演示
        // ============================================================
        printf("\n--- 4. StorageNVS 演示 ---\n");

        StorageNVS &storage = StorageNVS::requestInstance();

        // 4.1 设置参数
        storage.setLocalParam("demo_counter", 0);
        storage.setLocalParam("demo_text", "Hello Storage!");
        printf("[Storage] 设置参数: demo_counter=0\n");
        printf("[Storage] 设置参数: demo_text=\"Hello Storage!\"\n");

        // 4.2 读取参数
        int stored_counter;
        if (storage.getLocalParam("demo_counter", stored_counter)) {
            printf("[Storage] 读取参数: demo_counter=%d\n", stored_counter);
        }

        std::string stored_text;
        if (storage.getLocalParam("demo_text", stored_text)) {
            printf("[Storage] 读取参数: demo_text=\"%s\"\n", stored_text.c_str());
        }

        printf("\n========================================\n");
        printf("   应用运行中...                \n");
        printf("========================================\n\n");

        return true;
    }

    bool back() override {
        printf("\n[App] back() 被调用\n");
        return false;
    }

    bool pause() override {
        printf("\n[App] pause() 被调用\n");
        if (_timer && _timer->isValid()) {
            _timer->pause();
            printf("[Timer] 定时器已暂停\n");
        }
        return true;
    }

    bool resume() override {
        printf("\n[App] resume() 被调用\n");
        if (_timer && _timer->isValid()) {
            _timer->resume();
            printf("[Timer] 定时器已恢复\n");
        }
        return true;
    }

    bool close() override {
        printf("\n[App] close() 被调用\n");
        if (_timer) {
            _timer->reset();
            printf("[Timer] 定时器已重置\n");
        }
        return true;
    }

    bool deinit() override {
        printf("\n[App] deinit() 被调用\n");
        printf("[App] 应用已卸载\n");
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
    printf("   ESP-Brookesia 组件接口演示   \n");
    printf("========================================\n\n");

    // ============================================================
    // 1. 创建 Context（系统上下文）
    // ============================================================
    printf("\n--- 步骤 1: 创建 Context ---\n");

    Context::Config context_config = {
        .core_data = {
            .name = "Component Demo System",
            .screen_size = {
                .width = 360,
                .height = 360,
            },
        },
    };

    Context core;
    if (!core.begin(context_config)) {
        printf("[Main] 错误: Context 初始化失败\n");
        return;
    }
    printf("[Main] Context 初始化成功\n");

    // ============================================================
    // 2. 获取 Manager（应用管理器）
    // ============================================================
    printf("\n--- 步骤 2: 获取 Manager ---\n");

    Manager &manager = core.getManager();
    if (!manager.begin()) {
        printf("[Main] 错误: Manager 初始化失败\n");
        return;
    }
    printf("[Main] Manager 初始化成功\n");

    // ============================================================
    // 3. 创建并安装应用
    // ============================================================
    printf("\n--- 步骤 3: 创建并安装应用 ---\n");

    auto demo_app = std::make_shared<ComponentDemoApp>();

    int app_id = manager.installApp(demo_app.get());
    if (app_id < 0) {
        printf("[Main] 错误: 应用安装失败\n");
        return;
    }
    printf("[Main] 应用安装成功 (ID: %d)\n", app_id);

    // ============================================================
    // 4. 启动应用
    // ============================================================
    printf("\n--- 步骤 4: 启动应用 ---\n");

    if (!manager.startApp(app_id)) {
        printf("[Main] 错误: 应用启动失败\n");
        return;
    }
    printf("[Main] 应用启动成功\n");

    // ============================================================
    // 5. 显示应用信息
    // ============================================================
    printf("\n--- 步骤 5: 应用信息 ---\n");

    App *running_app = manager.getRunningAppById(app_id);
    if (running_app) {
        printf("[Main] 应用名称: %s\n", running_app->getName());
        printf("[Main] 应用 ID: %d\n", running_app->getId());
    }

    App *active_app = manager.getActiveApp();
    if (active_app) {
        printf("[Main] 活动应用: %s\n", active_app->getName());
    }

    uint8_t running_count = manager.getRunningAppCount();
    printf("[Main] 运行中应用数: %d\n", running_count);

    // ============================================================
    // 6. 主循环
    // ============================================================
    printf("\n========================================\n");
    printf("   应用运行中...                \n");
    printf("   按 Ctrl+C 停止              \n");
    printf("========================================\n\n");

    while (true) {
        vTaskDelay(pdMS_TO_TICKS(1000));
    }
}
