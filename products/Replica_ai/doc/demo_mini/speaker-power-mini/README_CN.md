# Speaker Power Mini

`speaker-power-mini` 聚焦 Speaker 项目的电源管理子系统，独立演示以下能力：

- 初始化电池监控系统；
- 实现电池电压、电流、电量（SOC）监测；
- 实现充电状态检测；
- 实现低电量告警和处理；
- 演示电源管理事件的处理机制。

## 目录结构

```
products/speaker-power-mini
├── CMakeLists.txt
├── README_CN.md
├── sdkconfig.defaults        # 推荐配置（esp32s3）
├── partitions.csv            # 分区配置
└── main
    ├── CMakeLists.txt
    ├── idf_component.yml     # 依赖声明（brookesia_core、echoear）
    ├── project_include.cmake
    └── main.cpp              # 入口应用（电源管理初始化 + 电池监控 + 低电量处理）
```

## 构建与烧录

1. 安装 ESP-IDF（>=5.5），执行 `source $IDF_PATH/export.sh`；
2. 进入项目并设置目标：
   ```bash
   cd products/speaker-power-mini
   idf.py set-target esp32s3
   ```
3. 使用推荐配置构建、烧录、监视：
   ```bash
   idf.py -D SDKCONFIG_DEFAULTS="sdkconfig.defaults" build
   idf.py flash monitor
   ```

> **提示**：首次烧录后，串口能看到电池监控初始化日志，以及定期的电池状态更新日志，包括电压、电流、电量和充电状态等信息。

