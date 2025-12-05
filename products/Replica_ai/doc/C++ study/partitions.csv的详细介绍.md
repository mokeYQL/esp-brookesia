![[Pasted image 20251204174232.png]]
## 1. 什么情况下使用这个文件

`partitions.csv` 是 ESP-IDF 项目中的**自定义分区表配置文件**，用于定义 Flash 存储器的分区结构。当满足以下条件时会使用此文件：

- **编译时配置**：在 `sdkconfig` 中设置了 `CONFIG_PARTITION_TABLE_CUSTOM=y`（从之前的 `sdkconfig.defaults` 中可以看到此配置）
- **项目构建**：在项目的 `CMakeLists.txt` 或 `idf_component_register` 中指定了使用自定义分区表
- **烧录过程**：使用 `idf.py flash` 或自定义烧录脚本时，会根据此文件生成实际的分区表二进制文件并烧录到设备中

## 2. 文件内容详细解释

`partitions.csv` 采用 CSV 格式，定义了 Flash 分区的各项属性。每行代表一个分区，字段含义如下：

| 字段名     | 类型   | 必须 | 解释说明                                                                 |
|------------|--------|------|--------------------------------------------------------------------------|
| **Name**   | 字符串 | 是   | 分区名称，用于标识分区，在代码中可通过名称访问分区                        |
| **Type**   | 字符串 | 是   | 分区类型，必须为以下值之一：<br>- `app`：应用程序分区<br>- `data`：数据分区 |
| **SubType**| 字符串 | 是   | 分区子类型，具体值取决于 Type 字段                                       |
| **Offset** | 十六进制| 否   | 分区起始偏移地址，不指定则自动计算                                      |
| **Size**   | 大小值 | 是   | 分区大小，支持 K/M 后缀（如 600K, 8M）或十六进制值（如 0x4000）          |
| **Flags**  | 字符串 | 否   | 分区标志，用于控制分区的特殊属性                                      |


#### 分区详细说明

| 分区名称      | 类型   | 子类型  | 大小     | 功能描述                                 |
|---------------|--------|---------|----------|------------------------------------------|
| **nvs**       | data   | nvs     | 0x4000   | 非易失性存储，用于保存系统配置和用户数据 |
| **otadata**   | data   | ota     | 0x2000   | OTA 更新元数据，记录当前运行的固件版本   |
| **phy_init**  | data   | phy     | 0x1000   | 物理层初始化数据，用于 WiFi/蓝牙 PHY     |
| **model**     | data   | spiffs  | 600K     | 存放语音唤醒模型（srmodels.bin）         |
| **factory**   | app    | factory | 8000K    | 工厂固件分区，存放初始固件               |
| **spiffs_data**| data  | spiffs  | 700K     | SPIFFS 文件系统，存放应用数据和资源      |
| **anim_emotion**| data | spiffs  | 1900K    | 存放表情动画资源                         |
| **anim_icon**  | data   | spiffs  | 1296K    | 存放图标动画资源                         |
| **anim_boot**  | data   | spiffs  | 640K     | 存放启动动画资源                         |

### 2.2 关键参数说明

#### 分区类型（Type）
- **app**：应用程序分区，用于存储固件镜像
- **data**：数据分区，用于存储非代码数据

#### 分区子类型（SubType）
- **nvs**：NVS 存储分区
- **ota**：OTA 更新元数据分区
- **phy**：PHY 初始化数据分区
- **factory**：工厂固件分区
- **spiffs**：SPIFFS 文件系统分区

#### 分区标志（Flags）
- **encrypted**：分区需要加密
- **read-only**：分区只读
- **no-map**：分区不映射到内存


## 二、标准分区名称定义
### 1. nvs - Non-Volatile Storage
- 定义位置 ：ESP-IDF 核心组件 ( components/nvs_flash/ )
- 功能 ：用于存储系统配置和用户数据的键值对存储
- 代码引用 ：通过 nvs_flash_init() 等 API 访问
- ESP-IDF 文档 ： https://docs.espressif.com/projects/esp-idf/en/latest/esp32/api-reference/storage/nvs_flash.html
### 2. otadata - OTA Data
- 定义位置 ：ESP-IDF OTA 组件 ( components/app_update/ )
- 功能 ：存储 OTA 更新的元数据，包括当前运行固件的版本和状态
- 代码引用 ：通过 esp_ota_get_boot_partition() 等 API 访问
- ESP-IDF 文档 ： https://docs.espressif.com/projects/esp-idf/en/latest/esp32/api-reference/system/ota.html
### 3. phy_init - PHY Initialization Data
- 定义位置 ：ESP-IDF PHY 组件 ( components/phy/ )
- 功能 ：存储 WiFi/蓝牙 PHY 层的初始化配置数据
- 代码引用 ：系统启动时自动加载，无需手动访问
- ESP-IDF 文档 ： https://docs.espressif.com/projects/esp-idf/en/latest/esp32/api-reference/network/esp_wifi.html
### 4. factory - Factory Firmware
- 定义位置 ：ESP-IDF 分区表规范
- 功能 ：存储初始固件镜像，用于首次启动或恢复出厂设置
- 代码引用 ：通过 esp_ota_get_partition_info() 等 API 访问
- ESP-IDF 文档 ： https://docs.espressif.com/projects/esp-idf/en/latest/esp32/api-reference/system/ota.html


### 3.3 在代码中的使用
Name 字段在代码中用于访问特定分区，例如：

```cpp
// 通过名称获取分区
esp_partition_t *partition = esp_partition_find_first(ESP_PARTITION_TYPE_DATA, 
                                                   ESP_PARTITION_SUBTYPE_DATA_SPIFFS, 
                                                   "model");
// 读取分区数据
esp_partition_read(partition, offset, buffer, size);
```

# 项目中5个参数怎么来的 
## spiffs_data
 - SPIFFS File System 定义位置：项目自定义，在 partitions.csv 中首次定义 功能：存储 SPIFFS 文件系统数据，包括音频文件、配置文件等 代码引用：在 main/CMakeLists.txt 中通过 spiffs_create_partition_image(spiffs_data ../spiffs FLASH_IN_PROJECT) 生成

## anim_emotion、anim_icon、anim_boot

依赖组件是

我需要搜索speaker产品目录下的构建配置文件，查找mmap资源生成的具体配置。
        
            
toolName: search_by_regex
            
status: success
          
query: mmap|anim_
          
我需要查看espressif__esp_mmap_assets组件的实现，这是生成mmap资源的关键组件。
        
            
toolName: view_files
            
status: success
          
            
filePath: e:\3.esp-brookesia\esp-brookesia\products\speaker\managed_components\espressif__esp_mmap_assets\idf_component.yml
          
我需要查看esp_mmap_assets组件的CMakeLists.txt文件，了解它如何生成资源文件。
        
            
toolName: view_files
            
status: success
          
            
filePath: e:\3.esp-brookesia\esp-brookesia\products\speaker\managed_components\espressif__esp_mmap_assets\CMakeLists.txt
          
我需要查看esp_mmap_assets组件的主要实现文件，了解它如何生成资源文件和配置烧录参数。
        
            
toolName: view_files
            
status: success
          
            
filePath: e:\3.esp-brookesia\esp-brookesia\products\speaker\managed_components\espressif__esp_mmap_assets\esp_mmap_assets.c
          
我需要搜索项目中关于esp_mmap_assets的配置，特别是在main目录下的构建配置文件中，以找到资源生成的具体位置。
        
            
toolName: search_by_regex
            
status: success
          
query: esp_mmap_assets
          
我需要搜索项目中关于mmap_build目录的生成配置，这是anim_emotion、anim_icon、anim_boot等资源生成的关键目录。
        
            
toolName: search_by_regex
            
status: success
          
query: mmap_build
          
我需要查看esp_mmap_assets组件的头文件，了解它的API和配置方式，以及anim_emotion、anim_icon、anim_boot等参数是如何生成的。
        
            
toolName: view_files
            
status: success
          
            
filePath: e:\3.esp-brookesia\esp-brookesia\products\speaker\managed_components\espressif__esp_mmap_assets\include\esp_mmap_assets.h
          
# anim_emotion、anim_icon、anim_boot 参数生成位置详解


# esp_mmap_assets 组件的资源生成机制

## 一、核心 CMake 函数

`esp_mmap_assets` 组件提供了一个关键的 CMake 函数 `spiffs_create_partition_assets`，用于生成和管理内存映射资源。这个函数对应于 `spiffs_create_partition_image`，但专门用于生成 `mmap` 资源。

## 二、函数定义与参数

### 1. 函数签名
```cmake
function(spiffs_create_partition_assets partition base_dir [OPTIONS])
```

### 2. 主要参数
| 参数名 | 类型 | 描述 |
|--------|------|------|
| `partition` | 必填 | 分区名称（如 `anim_emotion`、`anim_icon`、`anim_boot`） |
| `base_dir` | 必填 | 资源基础目录，包含原始资源文件 |
| `OPTIONS` | 可选 | 配置选项，如 `FLASH_IN_PROJECT`、`MMAP_SUPPORT_SJPG` 等 |

### 3. 关键选项
| 选项 | 描述 |
|------|------|
| `FLASH_IN_PROJECT` | 将资源添加到烧录流程中 |
| `MMAP_SUPPORT_SJPG` | 支持 SJPG 格式 |
| `MMAP_SUPPORT_SPNG` | 支持 SPNG 格式 |
| `MMAP_SUPPORT_QOI` | 支持 QOI 格式 |
| `MMAP_SPLIT_HEIGHT` | 资源分割高度 |

## 三、资源生成流程

### 1. 生成命令链
```
spiffs_create_partition_assets(partition base_dir FLASH_IN_PROJECT)
  → 调用 Python 工具：spiffs_assets_gen.py
    → 生成二进制资源文件：build/mmap_build/[type]/[name]/[name].bin
    → 生成资源描述文件：build/mmap_build/[type]/[name].json
    → 生成头文件：build/mmap_build/[type]/mmap_generate_[name].h
  → 调用 esptool_py_flash_to_partition()
    → 添加到 flasher_args.json
  → 最终通过 idf.py flash 烧录
```

### 2. 生成位置
- **二进制资源**：`${CMAKE_BINARY_DIR}/mmap_build/${base_dir_name}/${partition}/${partition}.bin`
- **描述文件**：`${CMAKE_BINARY_DIR}/mmap_build/${base_dir_name}/${partition}.json`
- **烧录配置**：`${CMAKE_BINARY_DIR}/flasher_args.json`

### 3. 烧录集成
```cmake
if(arg_FLASH_IN_PROJECT)
    set(assets_target "assets_${partition}_bin")
    
    if(arg_FLASH_APPEND_APP)
        set(assets_target "assets_${partition}_merge_bin")
        add_dependencies(app-flash ${assets_target})
    else()
        esptool_py_flash_to_partition(flash "${partition}" "${image_file}")
    endif()
    
    add_dependencies(flash ${assets_target})
endif()
```

## 四、与 spiffs_create_partition_image 的区别

| 特性   | `spiffs_create_partition_image` | `spiffs_create_partition_assets` |
| ---- | ------------------------------- | -------------------------------- |
| 用途   | 生成普通 SPIFFS 文件系统                | 生成内存映射资源                         |
| 输出格式 | SPIFFS 文件系统                     | 自定义二进制格式                         |
| 访问方式 | SPIFFS API                      | 内存映射直接访问                         |
| 资源类型 | 任意文件                            | 主要用于动画资源                         |
| 生成组件 | ESP-IDF 核心                      | `esp_mmap_assets` 组件             |
|      |                                 |                                  |

## 五、在项目中的使用

### 1. 调用示例
```cmake
# 示例：生成 anim_emotion 资源
spiffs_create_partition_assets(anim_emotion ../assets/emotion 
    FLASH_IN_PROJECT
    MMAP_SUPPORT_SJPG
    MMAP_SPLIT_HEIGHT 100
)
```

### 2. 生成结果
- **二进制文件**：`build/mmap_build/emotion/anim_emotion/anim_emotion.bin`
- **烧录配置**：添加到 `build/flasher_args.json`
- **自动烧录**：通过 `idf.py flash` 自动烧录到对应分区


# model 分区与 srmodels.bin 生成机制

## 一、核心生成组件

`model` 分区和 `srmodels.bin` 是由 **`espressif__esp-sr` 组件** 生成的，该组件负责 ESP 语音识别相关功能。

## 二、生成位置与配置

### 1. 生成代码位置
```cmake
# e:\3.esp-brookesia\esp-brookesia\products\speaker\managed_components\espressif__esp-sr\CMakeLists.txt
# 第 77-101 行
# Add model partition and flash srmodels.bin
if(CONFIG_PARTITION_TABLE_CUSTOM)
    partition_table_get_partition_info(size "--partition-name model" "size")
    partition_table_get_partition_info(offset "--partition-name model" "offset")

    if("${size}" AND "${offset}")
        set(MVMODEL_EXE ${COMPONENT_PATH}/model/movemodel.py)
        idf_build_get_property(build_dir BUILD_DIR)
        set(image_file ${build_dir}/srmodels/srmodels.bin)

        add_custom_command(
            OUTPUT ${image_file}
            COMMENT "Move and Pack models..."
            COMMAND python ${MVMODEL_EXE} -d1 ${SDKCONFIG} -d2 ${COMPONENT_PATH} -d3 ${build_dir}
            DEPENDS ${SDKCONFIG}
            VERBATIM)

        add_custom_target(srmodels_bin ALL DEPENDS ${image_file})
        add_dependencies(flash srmodels_bin)
        esptool_py_flash_to_partition(flash "model" "${image_file}")
    else()
        # 错误信息...
    endif()
endif()
```

### 2. 生成条件
- 仅当 `CONFIG_PARTITION_TABLE_CUSTOM` 为 `y` 时生成
- 必须在 `partitions.csv` 中定义 `model` 分区

## 三、生成流程

### 1. 生成命令链
```
esp-sr 组件 CMakeLists.txt (lines 77-101)
  → 检查 CONFIG_PARTITION_TABLE_CUSTOM
  → 从分区表获取 model 分区信息
  → 调用 Python 工具：movemodel.py
    → 生成 srmodels.bin：build/srmodels/srmodels.bin
  → 调用 esptool_py_flash_to_partition()
    → 添加到 flasher_args.json
  → 最终通过 idf.py flash 烧录
```

### 2. 生成位置
- **二进制文件**：`${build_dir}/srmodels/srmodels.bin`
- **烧录配置**：通过 `esptool_py_flash_to_partition` 添加到 `flasher_args.json`

## 四、与其他资源的区别

| 资源名称 | 生成组件 | 生成工具 | 生成位置 |
|----------|----------|----------|----------|
| `anim_emotion` | `esp_mmap_assets` | `spiffs_assets_gen.py` | `build/mmap_build/emotion/anim_emotion/` |
| `anim_icon` | `esp_mmap_assets` | `spiffs_assets_gen.py` | `build/mmap_build/icon/anim_icon/` |
| `anim_boot` | `esp_mmap_assets` | `spiffs_assets_gen.py` | `build/mmap_build/boot/anim_boot/` |
| `srmodels.bin` | `esp-sr` | `movemodel.py` | `build/srmodels/` |

## 五、总结

`model` 分区和 `srmodels.bin` 是：

1. **由 `espressif__esp-sr` 组件生成**：专门用于语音识别模型
2. **生成位置**：`build/srmodels/srmodels.bin`
3. **生成工具**：`movemodel.py` Python 脚本
4. **烧录机制**：通过 `esptool_py_flash_to_partition` 函数添加到烧录配置
5. **最终通过 `idf.py flash` 烧录**：与其他固件一起烧录到设备

这个机制与 `anim_emotion`、`anim_icon`、`anim_boot` 不同，它们是由 `esp_mmap_assets` 组件生成的，而 `srmodels.bin` 是由 `esp-sr` 组件专门生成的。

## 六、相关配置

### 1. 分区表配置
```csv
# partitions.csv
model,        data, spiffs,  ,        600K,
```

### 2. 生成条件
```cmake
# esp-sr/CMakeLists.txt
if(CONFIG_PARTITION_TABLE_CUSTOM)
    # 生成 srmodels.bin
endif()
```

### 3. 烧录集成
```cmake
# 添加到 flash 依赖
add_dependencies(flash srmodels_bin)
# 添加到烧录配置
esptool_py_flash_to_partition(flash "model" "${image_file}")
```

通过这种方式，`model` 分区和 `srmodels.bin` 被自动集成到构建和烧录流程中，实现了语音识别模型的自动管理和烧录。