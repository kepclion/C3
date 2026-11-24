#pragma once

#include <windows.h>
#include <string_view>

// ============================================================
// 颜色枚举
// ============================================================
enum ConsoleColor : WORD
{
    COLOR_BLACK   = 0,
    COLOR_BLUE    = 1,
    COLOR_GREEN   = 2,
    COLOR_CYAN    = 3,
    COLOR_RED     = 4,
    COLOR_PURPLE  = 5,
    COLOR_YELLOW  = 6,
    COLOR_WHITE   = 7,
    COLOR_GREY    = 8,
    COLOR_LBLUE   = 9,
    COLOR_LGREEN  = 10,
    COLOR_LCYAN   = 11,
    COLOR_LRED    = 12,
    COLOR_LPURPLE = 13,
    COLOR_LYELLOW = 14,
    COLOR_LWHITE  = 15,

    BCOLOR_BLUE    = 16,
    BCOLOR_GREEN   = 32,
    BCOLOR_CYAN    = 48,
    BCOLOR_RED     = 64,
    BCOLOR_PURPLE  = 80,
    BCOLOR_YELLOW  = 96,
    BCOLOR_WHITE   = 112,
    BCOLOR_GREY    = 128,
    BCOLOR_LBLUE   = 144,
    BCOLOR_LGREEN  = 160,
    BCOLOR_LCYAN   = 176,
    BCOLOR_LRED    = 192,
    BCOLOR_LPURPLE = 208,
    BCOLOR_LYELLOW = 224,
    BCOLOR_LWHITE  = 240,
};


inline HANDLE hStdin               = GetStdHandle(STD_INPUT_HANDLE);
inline HANDLE hConsole             = GetStdHandle(STD_OUTPUT_HANDLE);
inline WORD defaultConsoleColor    = COLOR_WHITE | COLOR_BLACK;
constexpr WORD initialConsoleColor = COLOR_WHITE | COLOR_BLACK;

// getColorFromTag 失败时的返回值
inline constexpr WORD INVALID_COLOR = 0xFFFF;


// ============================================================
// 控制台工具
// ============================================================
void ChangeConsoleDefaultColor();

HANDLE GetConsoleHandle();

void HideCursor();

void ShowCursor();

void ClearConsole();

void SetConsoleColor(WORD color);

void ResetConsoleColor();

void ResetConsoleColor_T();

// 将颜色标签名（不含尖括号，例如 "lyellow"）转换成控制台颜色。
// 未知标签返回 INVALID_COLOR。
WORD getColorFromTag(std::string_view tag) noexcept;

void SetConsoleCursorXY(int x, int y);

COORD GetConsoleCursorXY();

// 只把缓冲区对齐到当前窗口大小，不尝试修改窗口本身。
// Windows Terminal 会部分忽略 SetConsoleWindowInfo，强改会导致
// 「缓冲区 40 行 / 窗口 30 行」的错位。用户若需更大窗口，请手动拖拽。
// 返回 false 表示对齐失败（少见）。
bool AlignBufferToWindow();

void GetDesktopPath(TCHAR* path, size_t pathSize);
