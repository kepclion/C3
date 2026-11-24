#pragma once

// ---- 屏蔽 windows.h 的宏污染 ----
#ifndef NOMINMAX
#define NOMINMAX
#endif
#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif


#include <nlohmann/json.hpp>
#include <simpleini/SimpleIni.h>
#include <obfuscate.h>

#include <utility>
#include <vector>
#include <memory>
#include <string>
#include <string_view>
#include <fstream>
#include <sstream>
#include <iostream>
#include <iomanip>
#include <random>
#include <cmath>
#include <cstdio>
#include <cstdint>
#include <cstring>
#include <cstdlib>
#include <limits>
#include <algorithm>
#include <conio.h>
#include <thread>
#include <windows.h>
#include <shlobj.h>
#include <tchar.h>
#include <iostream>

// 检测按键按下（替代旧 KEY_DOWN 宏，类型安全、不污染宏命名空间）
inline bool KeyDown(const int vk) noexcept { return (GetAsyncKeyState(vk) & 0x8000) != 0; }

using json = nlohmann::json;
using ull  = unsigned long long;
using ll   = long long;
using cc   = const char *;

enum Prelude
{
    None     = 0,
    Agreed   = 1,
    LoggedIn = 2
};

// ============================================================
// 五个任务的 ID
//   顺序 = 游戏中的自然顺序，不要随意调整（会影响存档位序）
//   新增任务：在末尾追加，并更新 kTaskCount
// ============================================================

enum Tasks
{
    GameCheat      = 0,
    DecodeZip      = 1,
    FindQQClue     = 2,
    DocumentFault = 3,
    HeartKiller    = 4,

    Count
};


enum GameSurface
{
    MainMenu,

    Start,
    About,
    Help,
    SignIn,


    SiriusMenu,
    SiriusChatRoom,
    SiriusContract,
    SiriusMail,

    GameCheatTaskSurface,
    DecodeZipTaskSurface,
    FindQQClueTaskSurface,
    DocumentFaultTaskSurface,
    HeartKillerTaskSurface,

    MixMenu,
    MixChatRoom,
    MixContract,
    MixMail,

    HeartKillerTaskSurfaceMix,

    MidEnd,

    EXIT
};

enum ChatRoomStory
{
    Story_0,
    Story_1,
    Story_2,
    Story_3,
    Story_4,
};
