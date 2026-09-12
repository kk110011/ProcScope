# ============================================================================
#  src/CMakeLists.txt —— 定义可执行文件 target
#
#  【核心概念：target（目标）】
#  现代 CMake 里"一切皆 target"。target 之间的依赖用 target_xxx(...) 系列
#  命令声明，CMake 会自动把依赖所需的 **头文件路径、库文件、编译宏** 传递给
#  使用者。这就是所谓的 "target-based" / "Modern CMake"。
#
#  它取代了老式的全局污染写法：
#      include_directories(...)   ← 老写法：影响之后所有的 target
#      link_directories(...)      ← 老写法：影响之后所有的 target
#      link_libraries(...)        ← 老写法：影响之后所有的 target
#  新写法里每个 target 各管各的、互不干扰 —— Phase 2 引入 ImGui/GLFW 时，
#  你会真切体会到这一点有多重要（GUI 库的编译选项和采集模块完全不同）。
# ============================================================================


# ---- 1. 定义可执行文件 -----------------------------------------------------
# add_executable(<target 名> <源文件...>)
# target 名决定了最终产物名：procscope -> procscope.exe
add_executable(procscope
    main.cpp
)


# ---- 2. 链接第三方库 -------------------------------------------------------
# 【第一步：找包】find_package 让 CMake 去磁盘上搜索 fmt 的"配置文件"
# （fmt-config.cmake）。vcpkg 的工具链文件会拦截这次查找，把它重定向到
# vcpkg 自己安装的位置：build/vcpkg_installed/<triplet>/。
#     CONFIG   -> 只认 CMake 的 config 文件，不做老式 FindXXX.cmake 探测
#     REQUIRED -> 找不到就立刻报错，而不是静默跳过（务必加上）
#
# 【重要】少了这一步，下面的 fmt::fmt 就是一个"不存在的目标"：
# 编译时没有 include 路径，链接时找不到 fmt.lib，报的错还很难看懂。
find_package(fmt CONFIG REQUIRED)

# 【第二步：链接】fmt::fmt 这种 "带 :: 的名字" 叫 IMPORTED target（导入目标）：
# 它不是本项目编译出来的，而是 vcpkg 安装 fmt 时，在其 config 文件里替你
# 注册好的"描述信息"。链接它就一次性获得：
#     * fmt 的头文件搜索路径
#     * fmt.lib 的链接
#     * fmt 自身要求的编译宏
# 你既不用手写 include 路径，也不用写 #pragma comment(lib, "fmt.lib")。
#
# PRIVATE 的含义（链接可见性）：
#     PRIVATE   -> 只我自己用
#     PUBLIC    -> 我自己用，且依赖我的人也得用
#     INTERFACE -> 我自己不用，但依赖我的人必须用（纯头文件库常见）
# procscope 是 exe，没人依赖它，所以用 PRIVATE 就够了。
# 养成"能 PRIVATE 就 PRIVATE"的习惯，将来写库时不会踩坑。
target_link_libraries(procscope PRIVATE fmt::fmt)


# ---- 3. 平台相关的系统库 ---------------------------------------------------
# Phase 1 后面会用到的 Windows 库，先声明好：
#     psapi     -> EnumProcesses / GetProcessMemoryInfo
#     iphlpapi  -> 网络相关（Phase 2 可能用到）
# 注意：CreateToolhelp32Snapshot 属于 kernel32，MSVC 默认就会链接，无需显式写。
if (WIN32)
    target_link_libraries(procscope PRIVATE psapi iphlpapi)
endif()


# ---- 4. 编译选项 -----------------------------------------------------------
if (MSVC)
    target_compile_options(procscope PRIVATE
        /W4              # 警告等级 4（MSVC 常规范围内的最高级别）
        /permissive-     # 关闭宽松模式，向标准 C++ 靠拢
        /utf-8           # 【重要】源码按 UTF-8 读入，执行字符集也用 UTF-8
        /Zc:__cplusplus  # 【重要】让 __cplusplus 宏报告真实的 C++ 标准值
    )
else()
    target_compile_options(procscope PRIVATE -Wall -Wextra -Wpedantic)
endif()
