#include "allheader.h"
#include "Story.h"
#include "ConsoleUtils.h"
#include <unordered_map>


//把默认颜色设置改成当前样式
void ChangeConsoleDefaultColor()
{
    CONSOLE_SCREEN_BUFFER_INFO csbi;
    if (GetConsoleScreenBufferInfo(hConsole, &csbi)) defaultConsoleColor = csbi.wAttributes;
}

//获取控制台句柄
HANDLE GetConsoleHandle() { return hConsole; }

// 颜色标签名 -> 颜色值（哈希表查找）
WORD getColorFromTag(const std::string_view tag) noexcept
{
    static const std::unordered_map<std::string_view, WORD> kTable = {
        {"black", COLOR_BLACK},
        {"blue", COLOR_BLUE},
        {"green", COLOR_GREEN},
        {"cyan", COLOR_CYAN},
        {"red", COLOR_RED},
        {"purple", COLOR_PURPLE},
        {"yellow", COLOR_YELLOW},
        {"white", COLOR_WHITE},
        {"grey", COLOR_GREY},
        {"gray", COLOR_GREY},
        {"lblue", COLOR_LBLUE},
        {"lgreen", COLOR_LGREEN},
        {"lcyan", COLOR_LCYAN},
        {"lred", COLOR_LRED},
        {"lpurple", COLOR_LPURPLE},
        {"lyellow", COLOR_LYELLOW},
        {"lwhite", COLOR_LWHITE},
    };
    const auto it = kTable.find(tag);
    return it == kTable.end() ? INVALID_COLOR : it->second;
}

void HideCursor()
{
    constexpr CONSOLE_CURSOR_INFO info{.dwSize = 1, .bVisible = FALSE};
    SetConsoleCursorInfo(hConsole, &info);
}

void ShowCursor()
{
    constexpr CONSOLE_CURSOR_INFO info{.dwSize = 1, .bVisible = TRUE};
    SetConsoleCursorInfo(hConsole, &info);
}

//清理控制台
void ClearConsole()
{
    CONSOLE_SCREEN_BUFFER_INFO csbi;
    if (!GetConsoleScreenBufferInfo(hConsole, &csbi)) return;

    const DWORD cellCount = csbi.dwSize.X * csbi.dwSize.Y;
    constexpr COORD home{.X = 0, .Y = 0};
    DWORD written = 0;

    FillConsoleOutputCharacter(hConsole, ' ', cellCount, home, &written);
    FillConsoleOutputAttribute(hConsole, defaultConsoleColor, cellCount, home, &written);
    SetConsoleCursorPosition(hConsole, home);
}

//设置控制台颜色
void SetConsoleColor(const WORD color)
{
    SetConsoleTextAttribute(hConsole, color);
}

//重置到设置颜色
void ResetConsoleColor()
{
    SetConsoleTextAttribute(hConsole, defaultConsoleColor);
}

//重制到默认颜色
void ResetConsoleColor_T()
{
    SetConsoleTextAttribute(hConsole, initialConsoleColor);
}

//设置光标位置
void SetConsoleCursorXY(const int x, const int y)
{
    const COORD pos{.X = static_cast<SHORT>(x), .Y = static_cast<SHORT>(y)};
    SetConsoleCursorPosition(hConsole, pos);
}

//获取光标位置
COORD GetConsoleCursorXY()
{
    CONSOLE_SCREEN_BUFFER_INFO csbi;
    if (GetConsoleScreenBufferInfo(hConsole, &csbi)) return csbi.dwCursorPosition;
    return COORD{.X = -1, .Y = -1};
}

bool AlignBufferToWindow()
{
    CONSOLE_SCREEN_BUFFER_INFO csbi;
    if (!GetConsoleScreenBufferInfo(hConsole, &csbi)) return false;

    const SHORT winW = csbi.srWindow.Right - csbi.srWindow.Left + 1;
    const SHORT winH = csbi.srWindow.Bottom - csbi.srWindow.Top + 1;

    // 已经对齐：缓冲区 == 窗口，且窗口从顶部开始
    if (csbi.dwSize.X == winW && csbi.dwSize.Y == winH && csbi.srWindow.Top == 0)
        return true;

    // 窗口若不在顶部，先移回顶部（这样缓冲区才能缩到窗口大小）
    if (csbi.srWindow.Top != 0)
    {
        const SMALL_RECT top{
            .Left   = 0, .Top = 0,
            .Right  = static_cast<SHORT>(winW - 1),
            .Bottom = static_cast<SHORT>(winH - 1)
        };
        if (!SetConsoleWindowInfo(hConsole, TRUE, &top)) return false;
    }

    // 缓冲区 = 窗口
    return SetConsoleScreenBufferSize(hConsole, COORD{.X = winW, .Y = winH});
}


void GetDesktopPath(char* path, const size_t pathSize)
{
    if (SUCCEEDED(SHGetFolderPath(nullptr, CSIDL_DESKTOP, nullptr, SHGFP_TYPE_CURRENT, path)))
    {
        if (const size_t len = _tcslen(path); len > 0 and path[len - 1] != _T('\\'))
        {
            if (len < pathSize - 1)
                _tcscat(path, _T("\\"));
            else path[pathSize - 1] = _T('\0');
        } else
        {
            _tcscpy_s(path, pathSize, _T(""));
        }
    }
}
