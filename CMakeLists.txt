# ============================================================================
#  CMakeLists.txt —— 项目根配置
#  ProcScope · 本地系统监控仪
#
#  【新手须知】CMake 到底是干什么的？
#  CMake 本身**不是编译器**。它读取本文件里的"声明"，然后生成你所用
#  编译器能看懂的构建脚本：
#      对 Visual Studio 生成器 -> 生成 .sln / .vcxproj
#      对 Ninja 生成器        -> 生成 build.ninja
#  你从此不手写编译命令，只维护这份"声明"。好处是换编译器/换平台时，
#  只需要改 CMake 的调用参数，本文件基本不动。
#
#  本文件只负责【全局开关】，具体 target 定义在 src/CMakeLists.txt。
# ============================================================================


# ---- 1. 最低 CMake 版本 -----------------------------------------------------
# 声明所需的最低版本。低于它的 CMake 会直接报错退出，避免出现"语法能过、
# 但行为和新版本不一样"的隐性 bug。
# 写 3.30 的原因：CMakePresets.json 的 version 8（带 $schema 校验）需要 3.30+。
# 另外提醒：CMake 4.x 起**不再兼容** cmake_minimum_required(VERSION <3.5)
# 的老项目，如果以后拿到别人的老代码报这个错，把版本号抬高即可。
cmake_minimum_required(VERSION 3.30)


# ---- 2. 项目定义 -----------------------------------------------------------
# 第一个参数是项目名，会成为一个变量 PROJECT_NAME。
# LANGUAGES CXX 表示"本项目只包含 C++"，不启用 C/Fortran 等语言检查，
# 能稍微加快 configure 速度。
project(ProcScope
    VERSION     1.0.0
    DESCRIPTION "本地系统监控仪（类似 Windows 任务管理器）"
    LANGUAGES   CXX
)


# ---- 3. C++ 标准 -----------------------------------------------------------
# CMAKE_CXX_STANDARD 20        -> 所有 target 默认用 C++20
# CMAKE_CXX_STANDARD_REQUIRED  -> 编译器不支持 C++20 就直接报错，而不是静默降级
# CMAKE_CXX_EXTENSIONS OFF     -> 使用"纯标准"模式，禁用编译器私有扩展
#                                 对 MSVC 无影响；对 GCC 意味着用 -std=c++20
#                                 而不是 -std=gnu++20。工程上为了可移植性这么写。
set(CMAKE_CXX_STANDARD          20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS        OFF)


# ---- 4. 产物输出位置（可选但强烈建议） --------------------------------------
# 不设置的话，不同生成器会把 exe 丢到 build/Debug/、build/src/Debug/ 等各处，
# 找起来很烦。这里统一固定到 build/bin/<配置>/sysmon.exe。
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin")


# ---- 5. 引入子目录 ---------------------------------------------------------
# 真正的可执行文件 target 定义在 src/CMakeLists.txt 里。
# 拆分的理由：Phase 2 会增加 GUI、数据采集等多个 target，让根目录只保留
# "全局开关"，各 target 各管各的，避免单个文件无限膨胀。
add_subdirectory(src)
