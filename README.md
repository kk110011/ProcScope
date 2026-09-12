# ProcScope

本地系统监控仪（类似 Windows 任务管理器）—— 从零构建的 C++ 学习项目。

> 面向有算法竞赛基础、但缺少工程经验的开发者，按阶段逐步引入工程概念。

## 环境要求

| 依赖 | 版本 / 说明 |
|---|---|
| Windows | 10 / 11 x64 |
| 编译器 | Visual Studio 2022 Build Tools（MSVC + Windows SDK） |
| CMake | ≥ 3.25（本项目开发时使用 4.4.3） |
| vcpkg | 需设置环境变量 `VCPKG_ROOT` 指向 vcpkg 根目录（本项目为 `D:\vcpkg`） |

## 构建与运行

依赖采用 **vcpkg 清单模式**：依赖声明写在 `vcpkg.json` 里，CMake 配置阶段会
自动下载并编译缺失的依赖，**不需要手动执行 `vcpkg install`**。

```powershell
# 1. 配置（首次会自动下载并编译 fmt，需要几分钟）
cmake --preset msvc-x64

# 2. 构建 Debug
cmake --build --preset debug

# 3. 运行
.\build\bin\Debug\procscope.exe
```

换成 Release 构建：

```powershell
cmake --build --preset release
.\build\bin\Release\procscope.exe
```

## 目录结构

```
ProcScope/
├── CMakeLists.txt          根配置：全局开关（C++20、产物路径），不含具体 target
├── CMakePresets.json       预设参数：生成器、架构、vcpkg 工具链路径
├── vcpkg.json              依赖清单（清单模式）
├── .gitignore
└── src/
    ├── CMakeLists.txt      定义 procscope 可执行 target
    └── main.cpp            程序入口
```

拆分原则：根 `CMakeLists.txt` 只管"全局开关"，具体 target 放在 `src/` 下。
Phase 2 会新增 GUI 目标与数据采集模块，届时新增子目录即可，不必改动根文件。

## 开发路线

### Phase 1 —— V1.0 命令行版（无多线程、无 UI）

- [x] **Step 1** 工具链打通：CMake + vcpkg + fmt + Windows SDK 冒烟测试
- [ ] **Step 2** RAII 封装 Windows `HANDLE`（防句柄泄漏）
- [ ] **Step 3** `CreateToolhelp32Snapshot` 遍历进程，打印进程名与 PID
- [ ] **Step 4** 清屏 + `Sleep(1000)` 实现控制台动态刷新
- [ ] **Step 5** 系统内存使用率（可选）

### Phase 2 —— V2.0 桌面 UI + 多线程

- [ ] 生产者-消费者架构：采集线程写入共享数据区，UI 线程渲染
- [ ] Dear ImGui + GLFW + ImPlot 可视化
- [ ] 进程列表表头点击排序
- [ ] "结束进程"按钮

## 开发笔记

### Windows 头文件的"护身符"宏

必须在 `#include <windows.h>` **之前**定义：

```cpp
#define WIN32_LEAN_AND_MEAN  // 少包含冷门头，编译更快，减少命名冲突
#define NOMINMAX             // 禁止 windows.h 定义 min/max 宏，否则与 std::min/std::max 冲突
```

### 中文乱码的两道关

| 环节 | 解决手段 |
|---|---|
| 源码里的中文字节如何被编译器理解 | MSVC 编译选项 `/utf-8` |
| 输出的字节如何被控制台渲染 | 运行时调用 `SetConsoleOutputCP(CP_UTF8)` |

两者缺一不可。

### `__cplusplus` 宏

MSVC 默认报告 `199711L`（远古值），需 `/Zc:__cplusplus` 才准确。
