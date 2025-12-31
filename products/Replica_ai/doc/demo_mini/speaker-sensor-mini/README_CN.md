# Speaker Sensor Mini

`speaker-sensor-mini` 聚焦 Speaker 项目的传感器子系统，独立演示以下能力：

- 初始化 EchoEar BSP 完成触摸传感器配置；
- 实现触摸传感器事件处理，包括单击、长按等；
- 初始化 IMU 传感器，实现手势识别；
- 演示传感器事件与系统状态的交互。

## 目录结构

```
products/speaker-sensor-mini
├── CMakeLists.txt
├── README_CN.md
├── sdkconfig.defaults        # 推荐配置（esp32s3）
├── partitions.csv            # 分区配置
└── main
    ├── CMakeLists.txt
    ├── idf_component.yml     # 依赖声明（brookesia_core、echoear）
    ├── project_include.cmake
    └── main.cpp              # 入口应用（传感器初始化 + 事件处理）
```

## 构建与烧录

1. 安装 ESP-IDF（>=5.5），执行 `source $IDF_PATH/export.sh`；
2. 进入项目并设置目标：
   ```bash
   cd products/speaker-sensor-mini
   idf.py set-target esp32s3
   ```
3. 使用推荐配置构建、烧录、监视：
   ```bash
   idf.py -D SDKCONFIG_DEFAULTS="sdkconfig.defaults" build
   idf.py flash monitor
   ```

> **提示**：首次烧录后，串口能看到传感器初始化日志，触摸传感器或移动设备时能看到相应的事件日志，可据此验证传感器是否工作正常。

