# ProcScope —— 项目上下文（供 AI 助手 / 协作者阅读）

## 项目简介

本地系统监控仪（类似 Windows 任务管理器），从零构建的 C++ 学习项目。

- 仓库：https://github.com/kk110011/ProcScope.git
- 目标：按阶段平滑学习工程开发（CMake / vcpkg / 头文件分离 / RAII / 多线程 / GUI）

## 技术栈

| 层 | 选型 |
|---|---|
| 语言 | C++20 |
| 构建 | CMake（现代 target-based）+ CMakePresets |
| 依赖管理 | vcpkg（**清单模式**，依赖声明在 `vcpkg.json`） |
| 日志/格式化 | fmt（已用）；spdlog（后续引入） |
| Phase 2 UI | Dear ImGui + GLFW + ImPlot |
| 底层 | Windows API |

## 环境要求

| 工具 | 版本 / 位置 |
|---|---|
| MSVC | VS 2022 Build Tools（本项目实测 cl 14.44.35207） |
| Windows SDK | 10.0.26100.0 |
| CMake | ≥ 3.30 |
| vcpkg | 需设环境变量 `VCPKG_ROOT`（本项目为 `D:\vcpkg`） |

## 构建命令

```powershell
cmake --preset msvc-x64        # 配置（首次会由 vcpkg 自动下载并编译 fmt）
cmake --build --preset debug   # 构建 Debug
.\build\bin\Debug\procscope.exe
```

Release：`cmake --build --preset release` → `.\build\bin\Release\procscope.exe`

**注意**：这是 vcpkg 清单模式，**不需要手动执行 `vcpkg install`**。

## 目录约定

```
ProcScope/
├── CMakeLists.txt       根配置：只管全局开关（C++20、产物路径），不含具体 target
├── CMakePresets.json    预设参数：生成器、架构、vcpkg 工具链路径
├── vcpkg.json           依赖清单
├── README.md
└── src/
    ├── CMakeLists.txt   定义 procscope 可执行 target
    └── main.cpp
```

- 新增 target → 在 `src/CMakeLists.txt` 或新建子目录 + `add_subdirectory`
- 头文件与实现分离后，按模块建子目录（如 `src/core/`）

## 代码规范

- 现代 C++：智能指针、`std::filesystem`、避免裸指针
- 依赖传递一律用 `target_xxx` 系列命令，**不要**用 `include_directories()` / `link_directories()`
- 链接可见性：能 `PRIVATE` 就 `PRIVATE`
- 注释用中文，重点解释"为什么这么做"，而不只是"做了什么"

## 已知坑（务必遵守）

1. **必须先 `find_package(fmt CONFIG REQUIRED)`**，再 `target_link_libraries(... fmt::fmt)`。
   缺 `find_package` 会导致 `C1083: 无法打开包括文件 fmt/color.h`。
2. `CMakePresets.json` 使用 `version: 8`（version 6 不支持 `$schema` 字段）。
3. `#include <windows.h>` **之前**必须定义 `WIN32_LEAN_AND_MEAN` 和 `NOMINMAX`。
4. 中文不乱码需两道关：编译期 `/utf-8` + 运行期 `SetConsoleOutputCP(CP_UTF8)`。
5. MSVC 需 `/Zc:__cplusplus` 才会正确报告 `__cplusplus` 的值。
6. `MEMORYSTATUSEX` 使用前必须填 `dwLength = sizeof(ms)`。
7. 调用 `OpenProcess` 需要相应权限，权限不足会返回 `NULL`（错误码 5 = 拒绝访问）。

## 开发路线

### Phase 1 —— V1.0 命令行版（无多线程、无 UI）

- [x] **Step 1** 工具链打通：CMake + vcpkg + fmt + Windows SDK 冒烟测试
- [ ] **Step 2** RAII 封装 Windows `HANDLE`（防句柄泄漏）
- [ ] **Step 3** `CreateToolhelp32Snapshot` 遍历进程，打印进程名与 PID
- [ ] **Step 4** 清屏 + `Sleep(1000)` 控制台动态刷新
- [ ] **Step 5** 系统内存使用率（可选）

### Phase 2 —— V2.0 桌面 UI + 多线程

- [ ] 生产者-消费者架构：采集线程写共享数据区（加锁），UI 线程渲染
- [ ] Dear ImGui + GLFW + ImPlot
- [ ] 进程列表表头点击排序
- [ ] "结束进程"按钮
