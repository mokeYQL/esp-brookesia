# Speaker FS Mini

`speaker-fs-mini` 聚焦 Speaker 项目的文件系统子系统，独立演示以下能力：

- 初始化 SPIFFS 文件系统；
- 实现文件读写操作；
- 遍历目录和文件；
- 获取文件信息；
- 演示不同类型文件的处理。

## 目录结构

```
products/speaker-fs-mini
├── CMakeLists.txt
├── README_CN.md
├── sdkconfig.defaults        # 推荐配置（esp32s3）
├── partitions.csv            # 包含 spiffs_data 分区，内置示例文件
├── spiffs/                   # 包含示例文件
└── main
    ├── CMakeLists.txt
    ├── idf_component.yml     # 依赖声明（brookesia_core、echoear）
    ├── project_include.cmake
    └── main.cpp              # 入口应用（文件系统初始化 + 文件操作演示）
```

## 构建与烧录

1. 安装 ESP-IDF（>=5.5），执行 `source $IDF_PATH/export.sh`；
2. 进入项目并设置目标：
   ```bash
   cd products/speaker-fs-mini
   idf.py set-target esp32s3
   ```
3. 使用推荐配置构建、烧录、监视：
   ```bash
   idf.py -D SDKCONFIG_DEFAULTS="sdkconfig.defaults" build
   idf.py flash monitor
   ```

> **提示**：首次烧录会自动生成 `spiffs_data` 等资源分区，串口能看到文件系统初始化、文件操作以及目录遍历的日志，可据此验证文件系统是否工作正常。

