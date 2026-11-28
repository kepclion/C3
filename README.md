# C3 (KepCipher3)

一个基于 C++ 的 Windows 控制台剧情解谜游戏。玩家扮演 Sirius，通过登录、聊天室、邮件与任务流程推进剧情，完成五个任务并解锁不同结局分支。

## 环境要求

- Windows（源码使用了 `windows.h`、`conio.h`、`shlobj.h`）
- CMake 3.16+
- 支持 C++20 的编译器（MinGW-w64 或 MSVC 均可）

> 当前 `CMakeLists.txt` 已固定 `cmake_minimum_required(VERSION 3.16)` 与 `CMAKE_CXX_STANDARD 20`，无需手动修改。不能降到 C++17：源码使用了 `std::set::contains`、缩写函数模板、指定初始化器，Obfuscate 使用了 `consteval`，均为 C++20 特性。

## 第三方依赖（均为 header-only）

| 依赖 | 用途 | 许可证 | 放置位置 |
|---|---|---|---|
| nlohmann/json 3.12.0 | 解析 `Lines/lines.json` 剧情数据 | MIT | `third_party/nlohmann/` |
| SimpleIni 4.27 | 读写 `config.ini` 存档 | MIT（`LICENCE.txt` 须一同保留） | `third_party/simpleini/SimpleIni.h` |
| Obfuscate（Adam Yaxley 原版） | 编译期字符串混淆 | Unlicense（文件内自带声明） | `third_party/obfuscate.h` |

源码中以 `<nlohmann/json.hpp>`、`<simpleini/SimpleIni.h>`、`<obfuscate.h>` 形式引用。

- SimpleIni 的压缩包目录通常是 `simpleini-master`，必须重命名为 `simpleini`，否则 include 路径不匹配。
- nlohmann/json 的 `json.hpp` 会包含 `nlohmann/detail/` 下的头文件，**必须保留完整的 `third_party/nlohmann/` 目录**，不要只保留 `json.hpp`。
- `CMakeLists.txt` 已包含 `target_include_directories(C3 PRIVATE third_party)`，克隆后无需手动添加。

Obfuscate 上游地址：<https://github.com/adamyaxley/Obfuscate>（Unlicense，可自由使用与再分发）。该原版定义了 `OBF(...)` 宏，可直接用于 `Secret.h`，无需改成 `AY_OBFUSCATE`。

## 构建

### MSVC：默认动态 CRT

```powershell
cmake -S . -B build -G "Visual Studio 17 2022" -A x64
cmake --build build --config Release
```

产物位于 `build/Release/C3.exe`。该构建使用 `/MD`，运行时需要目标电脑安装 Microsoft Visual C++ Redistributable 2015–2022 x64。`Debug` 配置依赖不可再发行的调试 CRT，不能用于分发。

MSVC 构建会自动添加 `/utf-8`，无需手动设置源码编码选项。

### MSVC：静态 CRT（推荐用于免 Redistributable 分发）

```powershell
cmake -S . -B build-static -G "Visual Studio 17 2022" -A x64 -DC3_STATIC_MSVC_RUNTIME=ON
cmake --build build-static --config Release
```

`C3_STATIC_MSVC_RUNTIME` 默认 `OFF`，不影响现有构建。开启后：

- Release/RelWithDebInfo/MinSizeRel 使用 `/MT`（`MultiThreaded`）
- Debug 使用 `/MTd`（`MultiThreadedDebug`）

静态 CRT 版本不再依赖 `MSVCP140.dll`、`VCRUNTIME140*.dll` 与 `api-ms-win-crt-*.dll`，适合在未提供 VC++ Redistributable 的电脑上分发。代价是 exe 体积增大，且运行库安全修复需要重新构建发布。

### MinGW-w64 / CLion

```powershell
cmake -S . -B build-mingw -G "MinGW Makefiles" `
  -DCMAKE_C_COMPILER=gcc -DCMAKE_CXX_COMPILER=g++ `
  -DCMAKE_BUILD_TYPE=Release
cmake --build build-mingw
```

CMakeLists 会在 MinGW/GCC 下自动添加 `-static-libgcc -static-libstdc++`，并在构建后把 MinGW 运行库 DLL 复制到 exe 目录。使用 CLion 自带 MinGW + Ninja 时，应确保部署的 DLL 与构建 exe 的 MinGW 工具链一致。

## 运行与发布

运行时需保证可执行文件与 `Lines/` 目录处于同一级，因为程序从 `Lines/lines.json` 读取剧情：

```text
C3.exe
Lines/
  └─ lines.json
攻略.txt        ← 可选
```

`config.ini` 是首次运行时自动生成的存档，不要提交到仓库，也不要随发布包分发开发机上的版本。

发布建议：

- **MSVC `/MT` 静态 CRT**：分发 `C3.exe` + `Lines/lines.json` 即可，通常只需系统 DLL。
- **MSVC `/MD` 动态 CRT**：目标电脑需安装 VC++ Redistributable 2015–2022 x64。
- **MinGW Release**：随 exe 分发同一工具链中的 `libwinpthread-1.dll`、`libgcc_s_seh-1.dll`、`libstdc++-6.dll`。
- **不要分发 Debug 版**：Debug CRT 不可再发行。

可用以下命令检查实际依赖：

```powershell
objdump.exe -p C3.exe | findstr "DLL Name"
```

静态 MSVC Release 应只出现系统 DLL（实测仅 `KERNEL32.dll`）；若出现 `MSVCP140D.dll`、`VCRUNTIME140D.dll` 或 `ucrtbased.dll`，说明误用了 Debug 构建。

## 目录结构

```text
.
├── CMakeLists.txt        构建配置（含可选静态 CRT 开关）
├── copy_deps.cmake       遗留辅助脚本（含本机绝对路径，未被 CMakeLists 引用）
├── main.cpp              程序入口
├── Game.cpp/.h           游戏主流程与界面
├── Story.cpp/.h          剧情加载与渲染
├── Animation.cpp/.h      动画效果
├── AniUtils.cpp/.h       动画工具
├── ConsoleUtils.cpp/.h   控制台工具
├── SaveData.cpp/.h       存档读写
├── Secret.h              游戏内虚构凭据（非真实密码）
├── allheader.h           公共头文件与枚举定义
├── Lines/
│   ├── Line.cpp/.h       关卡逻辑
│   └── lines.json        剧情数据（必须提交）
├── third_party/          第三方头文件（完整保留）
└── README.md
```

`build/`、`build-*/` 与 `cmake-build-*/` 已被 `.gitignore` 忽略。

## 说明

- `Secret.h` 中的密码与用户名均为**游戏剧情内的虚构字符串**，不对应任何真实账号。
- `SaveData.h` 中 `MailboxAndTask` 里的全部邮箱（含 `19114514@stu.eju.cod`）及任务、事件均为**虚构的游戏设定**。`stu.eju.cod` 使用未委派的虚构顶级域，不可投递，**不代表任何真实个人或现实事件**。
- 存档格式版本见 `SaveData.h` 中的 `kSaveVersion`；修改字段含义后需递增该值。
