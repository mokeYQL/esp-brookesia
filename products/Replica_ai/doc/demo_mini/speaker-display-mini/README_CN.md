# Speaker Display Mini

`speaker-display-mini` 是一个可单独编译的 ESP-IDF 示例工程，用于复现 Brookesia Speaker 项目中的显示子系统：上电后初始化 EchoEar BSP、启动 LVGL、同步亮度参数，并在屏幕上渲染一个包含滑块和文本提示的最小 UI。通过该项目可以快速学习以下能力：

- 如何基于 `bsp/esp-bsp.h` 启动 LCD、背光以及 LVGL 任务；
- 使用 NVS 保存/恢复亮度设置；
- 在 LVGL 事件中实时调用 `bsp_display_brightness_set()` 并更新 UI。

## 目录结构

```
products/speaker-display-mini
├── CMakeLists.txt           # 顶层项目入口
├── sdkconfig.defaults       # 推荐配置（esp32s3 + EchoEar 板卡）
├── README_CN.md             # 本说明
└── main
    ├── CMakeLists.txt
    ├── idf_component.yml    # 依赖声明（esp-bsp、echoear、esp_lvgl_port）
    └── main.cpp             # 入口应用，展示亮度调节 UI
```

## 构建与烧录

1. 安装 ESP-IDF（>=5.5），设置好 `IDF_PATH` 与工具链；
2. 进入项目目录并设置目标：
   ```bash
   cd products/speaker-display-mini
   idf.py set-target esp32s3
   ```
3. 选择默认配置并编译、烧录、监视：
   ```bash
   idf.py -D SDKCONFIG_DEFAULTS="sdkconfig.defaults" build
   idf.py flash monitor
   ```

> **提示**：本项目依赖仓库内的 `products/speaker/common_components/echoear` 组件来驱动 EchoEar 硬件，使用其他硬件时需要替换 BSP 依赖与对应的 `sdkconfig.defaults`。

