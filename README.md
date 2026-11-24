# C3 (KepCipher3)

一个基于 C++ 的 Windows 控制台剧情解谜游戏。玩家扮演 Sirius，通过登录、聊天室、邮件与任务流程推进剧情，完成五个任务并解锁不同结局分支。

## 环境要求

- Windows（使用了 `windows.h`、`conio.h`、`shlobj.h`）
- CMake 3.16+ 与 **C++20** 编译器（MinGW-w64 / MSVC 均可）

> 本仓库的 `CMakeLists.txt` 已设为 `cmake_minimum_required(VERSION 3.16)` 与 `CMAKE_CXX_STANDARD 20`，可直接构建。**不要降到 C++17**：源码使用了 `std::set::contains`、缩写函数模板、指定初始化器，Obfuscate 使用了 `consteval`，均为 C++20 特性。

## 第三方依赖（均为 header-only）

| 依赖 | 用途 | 许可证 | 放置位置 |
|---|---|---|---|
| nlohmann/json 3.12.0 | 解析 `Lines/lines.json` 剧情数据 | MIT | `third_party/nlohmann/json.hpp` |
| SimpleIni 4.27 | 读写 `config.ini` 存档 | MIT（`LICENCE.txt` 须一同保留） | `third_party/simpleini/SimpleIni.h` |
| Obfuscate（Adam Yaxley 原版） | 编译期字符串混淆 | Unlicense（文件内自带声明） | `third_party/obfuscate.h` |

源码中以 `<nlohmann/json.hpp>`、`<simpleini/SimpleIni.h>`、`<obfuscate.h>` 形式引用。注意下载的 SimpleIni 压缩包目录名为 `simpleini-master`，**必须重命名为 `simpleini`**，否则 include 路径不匹配。精简原则：nlohmann 的 `json.hpp` 并非单文件版，它依赖 `nlohmann/adl_serializer.hpp` 及 `nlohmann/detail/` 下的整套头文件，**必须保留完整的 `nlohmann/` 目录**；SimpleIni 只需 `SimpleIni.h` 与 `LICENCE.txt`，`tests/` 等测试文件无需提交；Obfuscate 只需 `obfuscate.h`（许可证声明内嵌于文件末尾）。

克隆后需在 `CMakeLists.txt` 中加入：

```cmake
target_include_directories(C3 PRIVATE third_party)
```

Obfuscate 上游地址：<https://github.com/adamyaxley/Obfuscate>（Unlicense，可自由使用与再分发）。该原版为`AY_OBFUSCATE`宏，我将其更名为了 `OBF(...)` ，可直接用于 `Secret.h`。

## 构建与运行

```bash
cmake -S . -B build
cmake --build build
```

运行时需保证 **可执行文件与 `Lines/` 目录处于同一级**，因为程序从 `Lines/lines.json` 读取剧情：

```
C3.exe
Lines/
  └─ lines.json
config.ini        ← 首次运行自动生成，保存游戏进度
```

`config.ini` 是运行时存档，**不要提交到仓库**。

## 目录结构

```
.
├── CMakeLists.txt        构建配置
├── copy_deps.cmake       依赖复制辅助脚本（含本机绝对路径，克隆后需调整或删除）
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
├── third_party/          第三方头文件（需自行放置）
└── README.md
```

## 说明

- `Secret.h` 中的密码与用户名均为**游戏剧情内的虚构字符串**，不对应任何真实账号。
- `SaveData.h` 中 `MailboxAndTask` 里的全部邮箱（含 `19114514@stu.eju.cod`）及任务、事件均为**虚构的游戏设定**。`stu.eju.cod` 使用未委派的虚构顶级域，不可投递，**不代表任何真实个人或现实事件**。
- 存档格式版本见 `SaveData.h` 中的 `kSaveVersion`；修改字段含义后需递增该值。
