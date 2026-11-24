#pragma once
#include <obfuscate.h>

// 以下均为游戏剧情内的虚构凭据，不对应任何真实账号或密码。
// OBF(...) 为编译期字符串混淆宏，由 third_party/obfuscate.h 提供。

namespace Secrets
{
    inline char* kInitPassword        = OBF("690605");
    inline char* kSiriusLoginUser     = OBF("SiriusTi");
    inline char* kSiriusLoginPassword = OBF("Ares3301");
    inline char* kMixLoginUser     = OBF("M1xTri");
    inline char* kMixLoginPassword = OBF("DQ4k;FMmz6]jzy3$");
}
