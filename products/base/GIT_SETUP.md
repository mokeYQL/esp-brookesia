# Git 设置和提交指南

> **完成项目初始化后，按照以下步骤提交到 GitHub**

---

## ✅ 项目已准备就绪！

### 当前状态

项目已经完成基础结构搭建，包含：
- ✅ 项目主文档（README.md）
- ✅ 完整的学习文档系统（doc/）
- ✅ Hello World 示例代码（main/）
- ✅ 构建配置文件
- ✅ .gitignore 文件

---

## 📝 第 1 次提交：项目初始化

### 步骤 1：查看当前状态

```bash
cd /home/yql/esp32_exc/01_esp32s3_ai/esp-brookesia/products/base
git status
```

你应该看到：
```
位于分支 new_study
您的分支与上游分支 'origin/new_study' 一致。

未跟踪的文件:
  （使用 "git add <文件>..." 以包含要提交的内容）
        doc/
        main/
        .gitignore
        CMakeLists.txt
        partitions.csv
        sdkconfig.defaults
        README.md
```

### 步骤 2：添加文件到暂存区

```bash
# 添加所有文件
git add .

# 或者逐个添加
git add doc/
git add main/
git add README.md
git add CMakeLists.txt
git add partitions.csv
git add sdkconfig.defaults
git add .gitignore
```

### 步骤 3：查看暂存区

```bash
git status
```

### 步骤 4：创建提交

```bash
git commit -m "feat: 初始化 ESP-Brookesia 学习项目

- 创建项目基础结构
- 添加 Hello World 应用（main/main.cpp）
- 配置 CMake 构建系统
- 添加完整的文档系统（doc/）
  - 7 个核心文档（按数字排序）
  - 文档导航（README.md）
  - 学习进度追踪（progress.md）
- 添加 .gitignore 文件
- 更新项目主文档（README.md）

详见：COMMITS.md
"
```

### 步骤 5：推送到 GitHub

```bash
git push origin new_study
```

---

## 📝 之后的提交（每次完成任务后）

### 标准提交流程

```bash
# 1. 查看修改
git status

# 2. 添加修改的文件
git add <修改的文件>

# 3. 提交
git commit -m "<type>: <简短描述>

<详细描述（可选）>"

# 4. 推送
git push
```

### 提交示例

#### 示例 1：完成第一个学习任务

```bash
git add doc/progress.md doc/5.learning_notes.md
git commit -m "docs: 完成第一个学习任务

- 阅读 quick_start_guide.md
- 编译并运行 Hello World 应用
- 理解 Context、Manager、App 的作用
- 更新学习进度"
git push
```

#### 示例 2：修改代码

```bash
git add main/main.cpp
git commit -m "feat: 添加新的 UI 元素

- 添加按钮控件
- 添加事件处理
- 优化界面布局"
git push
```

---

## 📋 提交规范

### 提交类型（type）

| 类型 | 说明 | 示例 |
|------|------|------|
| `feat` | 新功能 | `feat: 添加按钮控件` |
| `fix` | 修复 bug | `fix: 修复编译错误` |
| `docs` | 文档更新 | `docs: 更新学习笔记` |
| `style` | 代码格式 | `style: 统一代码缩进` |
| `refactor` | 重构 | `refactor: 优化类结构` |
| `test` | 测试相关 | `test: 添加单元测试` |
| `chore` | 构建/工具 | `chore: 更新依赖` |

### 提交信息格式

```
<type>: <subject>

<body>

<footer>
```

- **subject**: 简短描述（不超过 50 字符）
- **body**: 详细描述（可选）
- **footer**: 关联 Issue（可选）

---

## 💡 最佳实践

### 1. 频繁提交
- ✅ 每完成一个小任务就提交
- ❌ 不要堆积太多修改再提交

### 2. 清晰的提交信息
- ✅ `feat: 添加按钮控件`
- ❌ `update`
- ❌ `fix bugs`

### 3. 同步文档
- 修改代码的同时更新相关文档
- 更新学习进度（doc/progress.md）

### 4. 提交前检查
```bash
# 查看修改内容
git diff

# 查看要提交的文件
git diff --cached

# 确认无误后再提交
git commit
```

---

## 🚀 快速提交脚本

创建一个脚本 `commit.sh`：

```bash
#!/bin/bash

# 获取提交类型和描述
read -p "Type (feat/fix/docs/refactor): " type
read -p "Description: " desc

# 添加所有修改
git add .

# 提交
git commit -m "$type: $desc"

# 推送
git push
```

使用方法：
```bash
chmod +x commit.sh
./commit.sh
```

---

## 📊 提交历史查看

```bash
# 查看最近 5 次提交
git log -5 --oneline

# 查看详细的提交历史
git log

# 查看某次提交的详细信息
git show <commit-id>
```

---

## 🆘 常见问题

### Q: 提交后想修改怎么办？

```bash
# 修改最后一次提交
git commit --amend

# 修改最后一次提交并修改提交信息
git commit --amend -m "新的提交信息"
```

### Q: 忘记添加某个文件怎么办？

```bash
# 添加遗漏的文件
git add <遗漏的文件>

# 修改最后一次提交
git commit --amend --no-edit
```

### Q: 想撤销上次提交怎么办？

```bash
# 撤销提交，保留修改
git reset HEAD~1

# 撤销提交，删除修改（危险！）
git reset --hard HEAD~1
```

---

## 🎯 下一步

1. ✅ 完成第一次提交（项目初始化）
2. 📖 按照 `doc/progress.md` 的任务开始学习
3. 💾 每完成一个任务就提交一次
4. 📝 记录学习心得到 `doc/5.learning_notes.md`

---

**准备好了吗？开始第一次提交吧！** 🚀

有任何问题随时问我！
