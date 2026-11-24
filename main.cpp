#include "allheader.h"
#include "ConsoleUtils.h"
#include "Story.h"
#include "Game.h"
#include "SaveData.h"

GameSurface SURFACE = MainMenu;

namespace
{
    // 低于此尺寸就提示玩家手动放大
    constexpr SHORT kMinCols = 80;
    constexpr SHORT kMinRows = 25;
}

SaveData save;

int main()
{
    SetConsoleOutputCP(CP_UTF8);

    // 1. 缓冲区对齐到当前窗口
    AlignBufferToWindow();

    // 2. 窗口太小，则请玩家手动调整
    CONSOLE_SCREEN_BUFFER_INFO csbi;
    if (GetConsoleScreenBufferInfo(hConsole, &csbi))
    {
        const SHORT winW = csbi.srWindow.Right - csbi.srWindow.Left + 1;
        const SHORT winH = csbi.srWindow.Bottom - csbi.srWindow.Top + 1;

        if (winW < kMinCols || winH < kMinRows)
        {
            ClearConsole();
            std::cout
                    << "当前终端窗口 " << winW << " x " << winH
                    << "，游戏建议至少 " << kMinCols << " x " << kMinRows << "。\n"
                    << "请拖拽窗口边框调整到合适大小，然后按任意键继续。\n"
                    << std::flush;
            _getch();
            ClearConsole();

            // 玩家拖完后重新对齐
            AlignBufferToWindow();
        }
    }

    ChangeConsoleDefaultColor();
    HideCursor();

    InitConfig();
    save = SaveData::Load();

    while (true) SurfaceShow(SURFACE, save);
}
