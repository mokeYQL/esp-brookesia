# ESP-Brookesia 学习项目

欢迎来到 ESP-Brookesia 的学习之旅！这个项目将带你从零开始深入学习 ESP-Brookesia 框架。

## 📂 项目结构

```
products/base/
├── README.md                    # 项目主文档
├── CMakeLists.txt              # 主构建配置
├── sdkconfig.defaults          # 默认配置
├── partitions.csv             # 分区表
├── .gitignore                 # Git 忽略文件
│
├── doc/                       # 📚 学习文档目录（所有学习资料）
│   ├── README.md             # 文档导航
│   ├── 1.quick_start_guide.md              # 快速上手指南
│   ├── 2.interface_cheatsheet.md            # 接口速查表
│   ├── 3.component_interfaces_guide.md      # 详细接口指南
│   ├── 4.component_interfaces_demo.cpp     # 接口演示代码
│   ├── 5.learning_notes.md                  # 学习笔记模板
│   └── 6.index.md                           # 学习资源总览
│
└── main/                      # 源代码目录
    ├── CMakeLists.txt        # 组件构建配置
    ├── idf_component.yml     # 组件依赖
    └── main.cpp             # 主程序
```

## 📚 学习计划

### 🆕 组件接口学习路线（推荐）

> **推荐**：从组件接口开始学习，能快速理解框架提供的功能

> **📁 所有学习文档都在 `doc/` 目录下**

#### 📖 学习资源
1. **[doc/1.quick_start_guide.md](doc/1.quick_start_guide.md)** - 快速上手指南（30 分钟）
   - 30 分钟快速入门
   - 环境配置和编译
   - 第一个应用运行

2. **[doc/2.interface_cheatsheet.md](doc/2.interface_cheatsheet.md)** - 接口速查表
   - 所有组件的方法签名
   - 快速查找接口
   - 适合开发时查阅

3. **[doc/3.component_interfaces_guide.md](doc/3.component_interfaces_guide.md)** - 详细组件接口指南
   - 5 大组件的详细说明
   - 每个组件的核心方法
   - 使用场景和示例代码

4. **[doc/4.component_interfaces_demo.cpp](doc/4.component_interfaces_demo.cpp)** - 接口使用演示
   - 展示如何使用各个组件接口
   - 完整可运行的示例代码

5. **[doc/5.learning_notes.md](doc/5.learning_notes.md)** - 学习笔记模板
   - 记录学习心得
   - 整理知识点

6. **[doc/6.index.md](doc/6.index.md)** - 学习资源总览
   - 完整的学习路线图
   - 知识图谱
   - 学习检查清单

#### 📋 组件学习顺序

```
第 1 步：阅读接口指南
    ↓ 阅读 component_interfaces_guide.md
    了解 5 大组件的职责和接口

第 2 步：参考速查表
    ↓ 随时查阅 interface_cheatsheet.md
    快速查找方法签名

第 3 步：运行演示示例
    ↓ 编译运行 component_interfaces_demo.cpp
    看到各个接口的实际效果

第 4 步：动手实践
    ↓ 创建自己的应用
    使用学到的接口
```

---

### 第 1 阶段：核心框架（3-4周）
- ✅ Lesson 1: Hello World 应用（本例）
- 🆕 组件接口学习（推荐先学）
- ⏳ Lesson 2: Context 和 Manager 深入
- ⏳ Lesson 3: App 生命周期详解
- ⏳ Lesson 4: Display 和事件系统
- ⏳ Lesson 5: 样式系统入门

### 第 2 阶段：应用开发（2-3周）
- ⏳ Lesson 6: 创建交互式 UI
- ⏳ Lesson 7: 多应用管理
- ⏳ Lesson 8: 自定义组件
- ⏳ Lesson 9: 资源管理

### 第 3 阶段：AI 功能（2-3周）
- ⏳ Lesson 10: AI Agent 入门
- ⏳ Lesson 11: 表情表达系统
- ⏳ Lesson 12: 语音交互
- ⏳ Lesson 13: 函数调用

### 第 4 阶段：产品集成（1-2周）
- ⏳ Lesson 14: Speaker 系统深入
- ⏳ Lesson 15: 完整产品构建

---

## 🎯 Lesson 1: Hello World 应用

### 目标
- 理解 ESP-Brookesia 的基本概念
- 掌握 App 生命周期
- 学会创建简单的 UI

### 核心概念

#### 1. Context（系统上下文）
Context 是整个框架的核心，管理所有资源：
- 显示设备
- 输入设备
- Manager（应用管理器）
- 事件系统

```cpp
Context core;
core.begin(context_config);  // 初始化
```

#### 2. Manager（应用管理器）
Manager 负责管理应用的生命周期：
- 安装/卸载应用
- 启动/停止应用
- 暂停/恢复应用
- 应用切换

```cpp
Manager &manager = core.getManager();
manager.begin();              // 初始化
manager.installApp(app);      // 安装
manager.startApp(app_id);     // 启动
```

#### 3. App（应用基类）
App 是所有用户应用的基类，定义了生命周期：

```
init() → run() → pause() → resume() → close() → deinit()
  ↑                                            ↓
  └──────────────────────────────────────────────┘
```

### 代码详解

#### 应用配置
```cpp
const char *getName() const {
    return "Hello World";
}

gui::StyleImage getLauncherIcon() const {
    return gui::StyleImage::IMAGE_NULL();
}
```
- `getName()`: 返回应用名称
- `getLauncherIcon()`: 返回启动器图标

#### 生命周期方法

**1. init()** - 应用安装时调用（只调用一次）
```cpp
bool init() override {
    printf("[HelloWorld] init() called\n");
    return true;
}
```
- 用于初始化应用资源
- 只在安装时调用一次
- 返回 true 表示成功

**2. run()** - 应用运行时调用（必须实现）
```cpp
bool run() override {
    // 获取当前屏幕
    lv_obj_t *screen = getActiveScreen();

    // 创建标签
    lv_obj_t *label = lv_label_create(screen);
    lv_label_set_text(label, "Hello, ESP-Brookesia!");
    lv_obj_center(label);

    return true;
}
```
- 应用的入口点
- 在这里创建 UI
- **必须实现**
- 返回 true 表示成功

**3. back()** - 用户按返回键时调用
```cpp
bool back() override {
    printf("[HelloWorld] back() called\n");
    return false;  // 返回 false 让系统处理
}
```
- 处理返回键事件
- 返回 true 表示已处理
- 返回 false 让系统处理

**4. pause()** - 应用暂停时调用
```cpp
bool pause() override {
    printf("[HelloWorld] pause() called\n");
    return true;
}
```
- 应用被切换到后台时调用
- 可以保存状态
- 可以暂停动画

**5. resume()** - 应用恢复时调用
```cpp
bool resume() override {
    printf("[HelloWorld] resume() called\n");
    return true;
}
```
- 应用从后台恢复时调用
- 恢复动画
- 刷新 UI

**6. close()** - 应用关闭时调用
```cpp
bool close() override {
    printf("[HelloWorld] close() called\n");
    return true;
}
```
- 应用被关闭时调用
- 清理临时资源
- 不需要删除 UI（系统自动清理）

**7. deinit()** - 应用卸载时调用（只调用一次）
```cpp
bool deinit() override {
    printf("[HelloWorld] deinit() called\n");
    return true;
}
```
- 应用被卸载时调用
- 清理所有资源
- 只在卸载时调用一次

### 编译和运行

```bash
# 1. 配置目标芯片
cd products/base
idf.py set-target esp32s3

# 2. 配置项目
idf.py menuconfig

# 3. 编译
idf.py build

# 4. 烧录和监控
idf.py -p /dev/ttyUSB0 flash monitor
```

### 预期输出

```
========================================
   ESP-Brookesia Learning Project
   Lesson 1: Hello World App
========================================

[Main] Context initialized successfully
[Main] Manager initialized successfully
[HelloWorld] init() called
[Main] App installed successfully (ID: 0)
[Main] App started successfully
[HelloWorld] run() called
[HelloWorld] Creating UI...
[HelloWorld] UI created successfully!

[Main] Application running...
```

### 练习任务

1. **修改应用名称**
   - 将 `getName()` 返回值改为你的名字

2. **修改文本内容**
   - 将标签文本改为其他内容

3. **修改样式**
   - 改变文本颜色
   - 改变字体大小

4. **测试生命周期**
   - 观察每个生命周期方法的调用顺序
   - 尝试多次启动和关闭应用

### 下一步

完成本课后，你将进入：
- **Lesson 2**: Context 和 Manager 深入学习
- **Lesson 3**: App 生命周期详解

---

## 📖 学习资源

### 官方文档
- ESP-Brookesia README: `core/brookesia_core/README_CN.md`
- 使用指南: `core/brookesia_core/docs/how_to_use_CN.md`
- Speaker 系统: `core/brookesia_core/docs/system_ui_phone_CN.md`

### 代码示例
- Speaker 产品: `products/speaker/main/`
- 示例应用: `apps/`

### 核心源码
- Context: `core/brookesia_core/systems/base/esp_brookesia_base_context.hpp`
- Manager: `core/brookesia_core/systems/base/esp_brookesia_base_manager.hpp`
- App: `core/brookesia_core/systems/base/esp_brookesia_base_app.hpp`

---

## 💡 学习建议

1. **循序渐进**：不要跳过任何课程
2. **动手实践**：每个课程都要亲自运行代码
3. **阅读源码**：对照源码学习效果更好
4. **记录笔记**：记录你的学习心得
5. **提出问题**：遇到问题随时问我

---

## 🚀 Git 提交指南

### 第一次提交

项目已经准备就绪，可以提交到 GitHub 了！

**详细步骤请查看**：[GIT_SETUP.md](GIT_SETUP.md)

### 提交记录

每次提交后请更新：[COMMITS.md](COMMITS.md)

---

## 📊 项目文件说明

| 文件/目录 | 用途 | 是否需要提交 |
|-----------|------|-------------|
| `README.md` | 项目主文档 | ✅ 是 |
| `GIT_SETUP.md` | Git 设置指南 | ✅ 是 |
| `COMMITS.md` | 提交记录 | ✅ 是 |
| `doc/` | 学习文档 | ✅ 是 |
| `main/` | 源代码 | ✅ 是 |
| `CMakeLists.txt` | 构建配置 | ✅ 是 |
| `sdkconfig.defaults` | 默认配置 | ✅ 是 |
| `partitions.csv` | 分区表 | ✅ 是 |
| `.gitignore` | Git 忽略规则 | ✅ 是 |
| `sdkconfig` | 当前配置（自动生成） | ❌ 否 |
| `build/` | 编译输出 | ❌ 否 |

---

## 📝 学习记录

在这里记录你的学习进度：

- [x] Lesson 1: Hello World 应用
- [ ] Lesson 2: Context 和 Manager 深入
- [ ] Lesson 3: App 生命周期详解
- [ ] Lesson 4: Display 和事件系统
- [ ] Lesson 5: 样式系统入门
- [ ] Lesson 6: 创建交互式 UI
- [ ] ...

---

祝你学习愉快！🎉
