# Git 提交记录

> **记录每次提交的目的和内容**

---

## 📝 提交历史

### 第 1 次：项目初始化
**日期**：2025-01-04

**提交信息**：
```
feat: 初始化 ESP-Brookesia 学习项目

- 创建项目基础结构
- 添加 main.cpp Hello World 应用
- 配置 CMake 构建系统
- 添加 ESP-IDF 组件依赖
```

**文件变更**：
- 新增：`CMakeLists.txt`
- 新增：`README.md`
- 新增：`main/` 目录及所有文件

---

### 第 2 次：添加学习文档
**日期**：2025-01-04

**提交信息**：
```
docs: 添加完整的学习文档系统

- 添加 doc/ 目录，包含所有学习资料
- 添加 7 个核心文档（按数字排序）
- 添加 .gitignore 文件
- 更新 README.md，添加文档导航
```

**文件变更**：
- 新增：`doc/README.md` - 文档导航
- 新增：`doc/1.quick_start_guide.md` - 快速上手指南
- 新增：`doc/2.interface_cheatsheet.md` - 接口速查表
- 新增：`doc/3.component_interfaces_guide.md` - 详细接口指南
- 新增：`doc/4.component_interfaces_demo.cpp` - 接口演示代码
- 新增：`doc/5.learning_notes.md` - 学习笔记模板
- 新增：`doc/6.index.md` - 学习资源总览
- 新增：`doc/progress.md` - 学习进度追踪
- 新增：`.gitignore`
- 修改：`README.md`

---

## 🎯 下一步提交计划

### 计划 1：完成第一个学习任务
**预计时间**：第 1 天

**提交信息**：
```
feat: 完成第一个学习任务

- 编译并运行 Hello World 应用
- 测试应用生命周期
- 记录学习笔记
- 更新学习进度
```

**预期变更**：
- 修改：`doc/progress.md`
- 修改：`doc/5.learning_notes.md`
- 可选：`main/main.cpp`（如果有修改）

---

### 计划 2：完成 GUI 组件学习
**预计时间**：第 2-3 天

**提交信息**：
```
feat: 完成 GUI 组件学习

- 学习 Style System
- 学习 LVGL Wrapper
- 运行接口演示代码
- 创建自己的 GUI 应用
```

**预期变更**：
- 修改：`doc/progress.md`
- 修改：`doc/5.learning_notes.md`
- 新增：`main/gui_demo.cpp`（可选）

---

## 📋 提交规范

### 提交信息格式
```
<type>: <subject>

<body>

<footer>
```

### Type 类型
- `feat`: 新功能
- `fix`: 修复 bug
- `docs`: 文档更新
- `style`: 代码格式调整
- `refactor`: 重构
- `test`: 测试相关
- `chore`: 构建过程或辅助工具的变动

### 示例
```
feat: 添加 StorageNVS 存储功能

- 实现参数保存
- 实现参数读取
- 添加错误处理

Closes #1
```

---

## 💡 提交建议

1. **小步提交**：每完成一个任务就提交一次
2. **清晰描述**：提交信息要清楚说明做了什么
3. **及时提交**：不要堆积太多修改再提交
4. **同步文档**：修改代码的同时更新文档

---

## 🔗 相关资源

- Git 提交规范：https://www.conventionalcommits.org/
- Git 教程：https://git-scm.com/docs/gittutorial
