#include "allheader.h"
#include "Animation.h"
#include "Story.h"
#include "SaveData.h"
#include "ConsoleUtils.h"
#include <filesystem>

/*1- CirLoading : duration label
 *2- LineLoading : header total filled empty type length delay
 *3- CPrint : text delay speaker hid
 *
 * 存档约定见 SaveData.h
 */

void InitConfig(const std::string& file)
{
    namespace fs = std::filesystem;
    if (fs::exists(file)) return;

    SaveData{}.Save(file);
}

// ---------------------------------------------------------------
// 分页：光标离窗口底部还剩 kPageBottomMargin 行时自动翻页
// ---------------------------------------------------------------

constexpr SHORT kPageBottomMargin = 4;

bool EnsureRoomForNewPage()
{
    CONSOLE_SCREEN_BUFFER_INFO csbi;
    if (!GetConsoleScreenBufferInfo(hConsole, &csbi)) return false;

    if (csbi.dwCursorPosition.Y < csbi.srWindow.Bottom - kPageBottomMargin)
        return false;

    SetConsoleCursorXY(0, csbi.srWindow.Bottom);
    SetConsoleTextAttribute(hConsole, COLOR_LBLUE);
    std::cout << "-- 按任意键继续 --" << std::flush;
    ResetConsoleColor_T();
    _getch();
    ClearConsole();
    return true;
}

void InitializeStory()
{
    ClearConsole();

    std::ifstream ifs("Lines/lines.json");
    if (!ifs.is_open())
    {
        PrintLn t;
        t.content="<lred>缺少 Lines/lines.json，请将其放在可执行文件同级的 Lines 目录</lred>";
        t.CPrint();
        _getch();
        exit(-1);
    }

    json lines;
    try
    {
        ifs >> lines;
    } catch (const json::exception& e)
    {
        std::cerr << "解析 lines.json 失败: " << e.what();
        return;
    }

    if (!lines.contains("start"))
    {
        std::cerr << "lines.json 缺少 \"start\" 节点";
        return;
    }

    for (auto& item: lines["start"])
    {
        EnsureRoomForNewPage(); // 每条记录处理前，确保当屏有空间
        ProcessStory(item);
    }
}

void ProcessStory(const nlohmann::basic_json<>& lineslist)
{
    if (const std::string type = lineslist.value("type", ""); type == "CirLoading")
    {
        CirLoading(lineslist.value("duration", 8ull),
                   lineslist.value("label", std::string{}));
    } else if (type == "LineLoading")
    {
        const std::string header = lineslist.value("header", "");
        const std::string filled = lineslist.value("filled", "*");
        const std::string empty  = lineslist.value("empty", "-");

        LineLoading(header.c_str(),
                    lineslist.value("total", 100ull),
                    filled.c_str(),
                    empty.c_str(),
                    lineslist.value("styletype", 0),
                    lineslist.value("length", 50ull),
                    lineslist.value("delayMs", 50));
    } else if (type == "CPrint")
    {
        PrintLn s;
        s.content = lineslist.value("text", "");
        s.delay   = lineslist.value("delay", 0);
        s.speaker = lineslist.value("speaker", "");
        s.hid     = lineslist.value("hid", 1);
        s.CPrint();
    } else if (type == "ReverseCPrint")
    {
        PrintLn s;
        s.content = lineslist.value("text", "");
        s.delay   = lineslist.value("delay", 0);
        s.speaker = lineslist.value("speaker", "");
        s.hid     = lineslist.value("hid", 1);
        s.ReverseCPrint();
    } else if (!type.empty())
    {
        std::cerr << "[Story] unknown type: " << type << '\n';
    }
}
