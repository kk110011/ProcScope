// ============================================================================
//  main.cpp —— 程序入口
//  ProcScope · Phase 1 / Step 1
//
//  【本步目标】这一版故意不写任何业务逻辑，只做一件事：
//  证明工具链完全打通。
//      1. CMake 能配置、能编译、能链接出 exe
//      2. vcpkg 提供的 fmt 能被找到并使用
//      3. Windows SDK 装好了，能调用真正的 Windows API
//  这一步跑通，后面的进程遍历才有意义。
// ============================================================================


// ---------------------------------------------------------------------------
// 【Windows 头文件的两个"护身符"宏】
// 必须在 #include <windows.h> **之前**定义，否则不生效。这是 Windows 开发
// 最常见的坑之一，现在养成习惯。
// ---------------------------------------------------------------------------
#define WIN32_LEAN_AND_MEAN  // 不包含 winsock.h / ole2.h 等冷门头：编译更快，
                             // 也避免和 Phase 2 要用的第三方库撞类型名。

#define NOMINMAX             // windows.h 会默认定义 min / max 两个 **宏**，
                             // 它们会把 std::min / std::max 直接做文本替换，
                             // 引发一堆看不懂的编译错误。这个宏禁止之。

#include <windows.h>

#include <fmt/color.h>
#include <fmt/format.h>

#include <string>

namespace {

// 编译期识别当前编译器。
// "_MSC_VER / __GNUC__" 这类符号叫"编译器特性宏"，由编译器在编译时预先定义。
// 用它配合 #if，就能写出跨编译器的代码。
std::string compilerInfo() {
#if defined(_MSC_VER)
    return fmt::format("MSVC {}.{}", _MSC_VER / 100, _MSC_VER % 100);
#elif defined(__clang__)
    return fmt::format("Clang {}", __clang_version__);
#elif defined(__GNUC__)
    return fmt::format("GCC {}.{}", __GNUC__, __GNUC_MINOR__);
#else
    return "Unknown";
#endif
}

// 把字节数格式化成人类可读的 GiB。
std::string formatGiB(unsigned long long bytes) {
    constexpr double kGiB = 1024.0 * 1024.0 * 1024.0;
    return fmt::format("{:.2f} GiB", static_cast<double>(bytes) / kGiB);
}

}  // namespace


int main() {
    // 让控制台按 UTF-8 解释我们打印的字节流，否则中文会变成乱码。
    //
    // 【坑】这和编译期的 /utf-8 选项是两码事，缺一不可：
    //     /utf-8              -> 解决"源码里的中文字节如何被编译器理解"
    //     SetConsoleOutputCP  -> 解决"输出的字节如何被控制台渲染"
    ::SetConsoleOutputCP(CP_UTF8);

    fmt::print(fg(fmt::color::cyan) | fmt::emphasis::bold,
               "=== ProcScope v1.0  ·  Phase 1 / Step 1 ===\n\n");

    // ---- 1. 证明 fmt 可用 --------------------------------------------------
    fmt::print("编译器        : {}\n", compilerInfo());

    // __cplusplus 是编译器预定义的宏，报告当前使用的 C++ 标准。
    // 【坑】MSVC 默认会让它显示 199711L（远古值），必须在 src/CMakeLists.txt
    // 里加 /Zc:__cplusplus 才会准确。所以这行同时也在验证那个开关生效了。
    fmt::print("C++ 标准      : {}\n", __cplusplus);

    // ---- 2. 证明 Windows SDK 可用 -----------------------------------------
    // GetSystemInfo 不需要任何权限，是验证"Windows 头文件和库接好没有"的
    // 最小 API。
    SYSTEM_INFO si{};
    ::GetSystemInfo(&si);
    fmt::print("逻辑处理器    : {} 个\n", si.dwNumberOfProcessors);

    // GlobalMemoryStatusEx 同样不需要权限。
    // ullTotalPhys / ullAvailPhys 这两个值，Phase 1 后面会用来算内存占用率。
    MEMORYSTATUSEX ms{};
    ms.dwLength = sizeof(ms);  // 【坑】必须填 dwLength，否则该 API 直接返回 FALSE
    if (::GlobalMemoryStatusEx(&ms) != 0) {
        fmt::print("物理内存      : 共 {}，可用 {}，占用率 {}%\n",
                   formatGiB(ms.ullTotalPhys),
                   formatGiB(ms.ullAvailPhys),
                   ms.dwMemoryLoad);
    } else {
        fmt::print(fg(fmt::color::red),
                   "GlobalMemoryStatusEx 失败，错误码 {}\n", ::GetLastError());
    }

    fmt::print(fg(fmt::color::green) | fmt::emphasis::bold,
               "\n[OK] 工具链打通：CMake + vcpkg + fmt + Windows SDK\n");
    fmt::print("下一步：封装 RAII 句柄类，用 CreateToolhelp32Snapshot 遍历进程。\n");

    return 0;
}
