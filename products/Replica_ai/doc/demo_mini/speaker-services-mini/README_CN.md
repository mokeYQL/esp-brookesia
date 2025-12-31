# Speaker Services Mini

`speaker-services-mini` 面向 Speaker 项目的系统服务层，演示如何在 Brookesia 框架中初始化 `StorageNVS`、监听事件并在 FreeRTOS 任务中安全地读写配置。通过它可以快速掌握：

- `StorageNVS::begin()` 的使用方式以及事件线程的运行特性；
- 如何将键值对写入 NVS 并实时收到 `Operation::UpdateNVS`、`Operation::EraseNVS` 等回调；
- 在后台任务中周期性刷新配置，模拟设备设置流程。

## 目录结构

```
products/speaker-services-mini
├── CMakeLists.txt           # 顶层项目入口
├── sdkconfig.defaults       # 推荐配置（esp32s3）
├── README_CN.md             # 本说明
└── main
    ├── CMakeLists.txt
    ├── idf_component.yml    # 依赖声明（esp-idf、brookesia_core）
    └── main.cpp             # 入口应用，演示 StorageNVS 服务
```

## 构建与烧录

1. 安装 ESP-IDF（>=5.5），设置好 `IDF_PATH` 与工具链；
2. 进入项目目录并设置目标：
   ```bash
   cd products/speaker-services-mini
   idf.py set-target esp32s3
   ```
3. 选择默认配置并编译、烧录、监视：
   ```bash
   idf.py -D SDKCONFIG_DEFAULTS="sdkconfig.defaults" build
   idf.py flash monitor
   ```

> **提示**：本示例依赖仓库中的 `core/brookesia_core`，无需连接屏幕即可运行；如需扩展到真实产品，可在此基础上继续接入显示、音频等模块。

