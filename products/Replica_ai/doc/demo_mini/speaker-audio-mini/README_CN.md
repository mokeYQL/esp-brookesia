# Speaker Audio Mini

`speaker-audio-mini` 聚焦 Speaker 项目的音频子系统，独立演示以下能力：

- 复用 EchoEar BSP 完成 I2S/Codec 初始化（`audio_manager_init`）；
- 利用 `StorageNVS` 监听系统音量键值，并在回调中调用 `esp_codec_dev_set_out_vol()`；
- 打开 `audio_prompt`，从 `/spiffs` 分区播放示例 mp3 验证整条音频链路。

## 目录结构

```
products/speaker-audio-mini
├── CMakeLists.txt
├── README_CN.md
├── sdkconfig.defaults        # 推荐配置（esp32s3）
├── partitions.csv            # 包含 spiffs_data 分区，内置 mp3 提示音
├── spiffs/                   # 直接复用 Speaker 项目的音频资源
└── main
    ├── CMakeLists.txt
    ├── idf_component.yml     # 依赖声明（brookesia_core、echoear、gmf_ai_audio）
    ├── project_include.cmake
    └── main.cpp              # 入口应用（音频初始化 + NVS 事件 + 提示音播放）
```

## 构建与烧录

1. 安装 ESP-IDF（>=5.5），执行 `source $IDF_PATH/export.sh`；
2. 进入项目并设置目标：
   ```bash
   cd products/speaker-audio-mini
   idf.py set-target esp32s3
   ```
3. 使用推荐配置构建、烧录、监视：
   ```bash
   idf.py -D SDKCONFIG_DEFAULTS="sdkconfig.defaults" build
   idf.py flash monitor
   ```

> **提示**：首次烧录会自动生成 `spiffs_data` 等资源分区，串口能看到音量键写入、事件通知以及 mp3 播放日志，可据此验证音频链路是否工作正常。

