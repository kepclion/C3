#include "Animation.h"
#include "allheader.h"
#include "ConsoleUtils.h"
#include "AniUtils.h"
#include <algorithm>
#include <string_view>






// ============================================================
// 颜色标签解析：<color>text</color>
// ============================================================
namespace
{
    struct ParsedTag
    {
        bool valid     = false;
        bool isClose   = false;
        WORD color     = 0;
        size_t nextPos = 0;
    };

    ParsedTag TryParseColorTag(const std::string_view content, const size_t pos)
    {
        ParsedTag r;
        if (pos >= content.size() || content[pos] != '<') return r;

        const size_t close = content.find('>', pos + 1);
        if (close == std::string_view::npos) return r;

        std::string_view tag = content.substr(pos + 1, close - pos - 1);
        if (tag.empty()) return r;

        if (tag.front() == '/')
        {
            tag.remove_prefix(1);
            if (getColorFromTag(tag) == INVALID_COLOR) return r;
            r.valid   = true;
            r.isClose = true;
            r.nextPos = close + 1;
            return r;
        }

        const WORD color = getColorFromTag(tag);
        if (color == INVALID_COLOR) return r;

        r.valid   = true;
        r.color   = color;
        r.nextPos = close + 1;
        return r;
    }
}


// ============================================================
// 加载动画
// ============================================================
void CirLoading(const ull duration, const std::string_view label)
{
    HideCursor();
    ull i = 0;
    while (i < duration)
    {
        constexpr int frameCount = 4;
        for (int f = 0; f < frameCount && i < duration; ++f)
        {
            const char* frames[] = {"-", "\\", "|", "/"};
            printf("\r%s %.*s", frames[f],
                   static_cast<int>(label.size()), label.data());
            fflush(stdout);
            Sleep(50);
            ++i;
        }
    }
    printf("\r%*s\r", static_cast<int>(DisplayWidth(label) + 3), "");
    ShowCursor();
}

// ============================================================
// 线性进度条
// ============================================================
void LineLoading(const char* header, const ull total,
                 const char* filled, const char* empty,
                 const int type, const ull length, const int delayMs)
{
    HideCursor();
    for (ull step = 0; step <= total; ++step)
    {
        const double progress = static_cast<double>(step) / static_cast<double>(total);
        const double t        = lerp(type, progress);
        ull filledCount       = static_cast<ull>(t * static_cast<double>(length));
        if (filledCount > length) filledCount = length;

        printf("\r%s ", header);
        for (ull i = 0; i < length; ++i)
            printf("%s", i < filledCount ? filled : empty);

        printf(" %3d%%", static_cast<int>(progress * 100));
        fflush(stdout);
        Sleep(delayMs);
    }
    printf("\n");
    ShowCursor();
}

// ============================================================
// PrintLn::CPrint
// ============================================================
void PrintLn::CPrint() const
{
    if (hid) HideCursor();

    // 无控制台时剥离颜色标签，输出纯文本
    if (hConsole == INVALID_HANDLE_VALUE)
    {
        size_t i = 0;
        while (i < content.size())
        {
            if (content[i] == '<')
            {
                if (const auto tag = TryParseColorTag(content, i); tag.valid)
                {
                    i = tag.nextPos;
                    continue;
                }
            }
            std::cout << content[i++];
        }
        std::cout << std::flush;
        return;
    }

    if (!speaker.empty())
    {
        SetConsoleTextAttribute(hConsole, COLOR_LBLUE);
        printf("%s: ", speaker.c_str());
        SetConsoleTextAttribute(hConsole, defaultConsoleColor);
    }

    size_t pos = 0;
    while (pos < content.length())
    {
        if (content[pos] == '<')
        {
            if (const auto [valid, isClose, color, nextPos] = TryParseColorTag(content, pos); valid)
            {
                SetConsoleTextAttribute(
                    hConsole,
                    isClose ? defaultConsoleColor : color);
                pos = nextPos;
                continue;
            }
        }

        const char ch = content[pos];

        if (ch == '~')
        {
            Sleep(delay * 5);
            ++pos;
            continue;
        }

        if (ch == '\n' && !speaker.empty())
        {
            putchar('\n');
            std::cout << std::string(speaker.size() + 2, ' ');
            ++pos;
            continue;
        }

        if (delay == 0)
        {
            putchar(ch);
            ++pos;
        } else
        {
            const int charLen   = utf8CharLen(static_cast<unsigned char>(ch));
            const auto utf8Char = content.substr(pos, charLen);
            typeUtf8Char(utf8Char, delay);
            pos += charLen;
        }
    }

    if (!hid) ShowCursor();
}

// ============================================================
// 效果：正反打印
// ============================================================
void CPrintThenBack(const CPrintThenBackParams& p)
{
    const auto [X, Y] = GetConsoleCursorXY();

    PrintLn temp = p.forward;
    temp.delay   = p.delayForward;
    temp.CPrint();
    SetConsoleCursorXY(X, Y); // ← 回初始 X（原来是硬编码 0）
    Sleep(p.waitForward);

    temp.delay = p.delayBack;
    temp.ReverseCPrint();
    Sleep(p.waitBack);

    const size_t contentW = DisplayWidth(p.forward.content);
    const size_t speakerW = p.forward.speaker.empty()
                                ? 0
                                : p.forward.speaker.size() + 2;

    // 从行首清到「内容右端」（含起始 X 偏移，避免短内容残留）
    printf("\r%*s\r", static_cast<int>(X + contentW + speakerW), "");
    fflush(stdout);

    temp.content = p.finalText;
    temp.delay   = p.delayFinal;
    temp.CPrint();
}

// ============================================================
// 效果：滚屏打印
// ============================================================
void RollCPrint(const std::vector<std::string>& items,
                const int windowLines,
                const int blankLines,
                const int delayMs,
                const int loop)
{
    if (items.empty()) return;

    const auto [X, Y] = GetConsoleCursorXY();

    PrintLn s;
    s.delay = 0;
    s.hid   = 1;

    size_t maxW = 0;
    for (const auto& item: items) maxW = std::max(maxW, DisplayWidth(item));

    std::vector strs(blankLines, std::string(maxW, ' '));
    strs.insert(strs.end(), items.begin(), items.end());

    const ull size = strs.size();
    ull start      = 0;
    ull end        = (start + windowLines) % size;

    auto Draw = [&]
    {
        SetConsoleCursorXY(X, Y);
        for (ull pt = start; pt != end; pt = (pt + 1) % size)
        {
            s.CPrint(strs[pt]);
            if (const size_t w = DisplayWidth(strs[pt]); w < maxW)
                printf("%s", std::string(maxW - w, ' ').c_str());
            printf("\n");
        }
    };

    for (;;)
    {
        Draw();
        ++start;

        if (loop == 0 && start >= size) break;

        end   = (start + windowLines) % size;
        start %= size;

        if (delayMs > 0) Sleep(delayMs);
    }
    SetConsoleCursorXY(X, Y);
}


// ============================================================
// CChoice 内部
// ============================================================
namespace
{
    // 前缀 -> ChoiceKind，原地剥离
    ChoiceKind StripChoicePrefix(std::string& text)
    {
        if (text.starts_with("<none>"))
        {
            text.erase(0, 6);
            return ChoiceKind::Blank;
        }
        if (text.starts_with("<title>"))
        {
            text.erase(0, 7);
            return ChoiceKind::Title;
        }
        if (text.starts_with("<invalid>"))
        {
            text.erase(0, 9);
            return ChoiceKind::Disabled;
        }
        return ChoiceKind::Normal;
    }

    struct ChoiceEntry
    {
        std::string text;
        ChoiceKind kind = ChoiceKind::Normal;
        std::string prefix;
        int resultIdx = -1;
    };

    struct PreparedChoices
    {
        ChoiceLayout layout;
        std::vector<ChoiceEntry> entries;
        std::vector<size_t> highlightable;
    };

    PreparedChoices PrepareChoices(const std::vector<ChoiceItem>& raw,
                                   const std::string& speaker,
                                   const int style)
    {
        PreparedChoices P;
        P.layout.vertical   = isVertical(style);
        P.layout.indent     = getIndent(speaker);
        P.layout.decorWidth = (getLeftDecor(style) != '\0' ? 2 : 0)
                              + (getRightDecor(style) != '\0' ? 2 : 0);

        P.entries.reserve(raw.size());
        for (const auto& [text, kind]: raw)
        {
            ChoiceEntry e;
            e.text = text;
            e.kind = kind;
            P.entries.push_back(std::move(e));
        }

        const uint32_t numMode = numModeOf(style);
        const bool useNum      = numMode != ChoiceStyle::NumNone;

        std::vector<std::string> nums(P.entries.size());
        size_t maxNumW = 0;
        int counter    = 0;

        for (size_t i = 0; i < P.entries.size(); ++i)
        {
            auto& e = P.entries[i];
            if (e.kind != ChoiceKind::Normal && e.kind != ChoiceKind::Disabled) continue;

            const int v = ++counter;
            if (useNum)
            {
                switch (numMode)
                {
                    case ChoiceStyle::NumArabic: nums[i] = std::to_string(v);
                        break;
                    case ChoiceStyle::NumRomanUpper: nums[i] = toRoman(v, true);
                        break;
                    case ChoiceStyle::NumRomanLower: nums[i] = toRoman(v, false);
                        break;
                    default: break;
                }
                maxNumW = std::max(maxNumW, nums[i].size());
            }

            e.resultIdx = counter - 1;
            P.highlightable.push_back(i);
        }

        P.layout.prefixWidth = useNum ? maxNumW + 2 : 0;
        for (size_t i = 0; i < P.entries.size(); ++i)
        {
            auto& e = P.entries[i];
            if (e.kind != ChoiceKind::Normal && e.kind != ChoiceKind::Disabled) continue;

            if (!useNum)
            {
                e.prefix.clear();
                continue;
            }
            e.prefix = std::string(maxNumW - nums[i].size(), ' ') + nums[i] + ". ";
        }

        if (!P.layout.vertical)
        {
            P.layout.horizontalWidth = 0;
            bool first               = true;
            for (const auto& e: P.entries)
            {
                if (e.kind == ChoiceKind::Blank) continue;
                if (!first) P.layout.horizontalWidth += CHOICE_GAP.size();
                first = false;

                if (e.kind != ChoiceKind::Title)
                    P.layout.horizontalWidth += P.layout.prefixWidth + P.layout.decorWidth;
                P.layout.horizontalWidth += DisplayWidth(e.text);
            }
        }

        return P;
    }

    void PrintEntry(const ChoiceEntry& e, const bool isSelected,
                    const int notselected, const int selected, const int style)
    {
        if (e.kind == ChoiceKind::Blank) return;

        const bool isTitle = e.kind == ChoiceKind::Title;
        const char left    = isTitle ? '\0' : getLeftDecor(style);
        const char right   = isTitle ? '\0' : getRightDecor(style);

        SetConsoleTextAttribute(hConsole, isSelected ? selected : notselected);

        PrintLn s;
        if (left == '\0' && right == '\0')
            s.content = e.text;
        else if (left == '\0')
            s.content = e.text + " " + std::string(1, right);
        else if (right == '\0')
            s.content = std::string(1, left) + " " + e.text;
        else
            s.content = std::string(1, left) + " " + e.text + " " + std::string(1, right);

        s.CPrint();
        ResetConsoleColor_T();
    }

    void RenderChoices(const PreparedChoices& P,
                       const COORD startPos,
                       const ll highlightIdx,
                       const int notselected, const int selected,
                       const int style)
    {
        SetConsoleCursorXY(startPos.X, startPos.Y);

        if (P.layout.vertical)
        {
            for (size_t i = 0; i < P.entries.size(); ++i)
            {
                const auto& e = P.entries[i];

                if (e.kind == ChoiceKind::Blank)
                {
                    std::cout << '\n';
                    continue;
                }

                if (i > 0) std::cout << P.layout.indent;
                if (e.kind != ChoiceKind::Title) std::cout << e.prefix;

                PrintEntry(e, static_cast<ll>(i) == highlightIdx,
                           notselected, selected, style);
                std::cout << '\n';
            }
        } else
        {
            bool first = true;
            for (size_t i = 0; i < P.entries.size(); ++i)
            {
                const auto& e = P.entries[i];
                if (e.kind == ChoiceKind::Blank) continue;

                if (!first) std::cout << CHOICE_GAP;
                first = false;

                if (e.kind != ChoiceKind::Title) std::cout << e.prefix;
                PrintEntry(e, static_cast<ll>(i) == highlightIdx,
                           notselected, selected, style);
            }
            std::cout << '\n';
        }

        std::cout << std::flush;
    }

    void ClearChoiceArea(const COORD startPos, const PreparedChoices& P)
    {
        if (P.entries.empty()) return;

        // indent 空 => 无 speaker；否则 indent.size() == speaker.size() + 2
        const size_t speakerWidth = P.layout.indent.size();

        if (P.layout.vertical)
        {
            for (size_t i = 0; i < P.entries.size(); ++i)
            {
                const auto& e  = P.entries[i];
                const size_t w = speakerWidth
                                 + (e.kind != ChoiceKind::Title ? P.layout.prefixWidth : 0)
                                 + (e.kind == ChoiceKind::Normal or e.kind == ChoiceKind::Disabled
                                        ? P.layout.decorWidth
                                        : 0)
                                 + DisplayWidth(e.text);

                SetConsoleCursorXY(0, static_cast<int>(startPos.Y + i));
                std::cout << std::string(w, ' ');
            }
        } else
        {
            SetConsoleCursorXY(0, startPos.Y);
            std::cout << std::string(speakerWidth + P.layout.horizontalWidth, ' ');
        }
        std::cout << std::flush;
    }

    // 真正的实现：接收结构化条目
    void CChoiceImpl(const std::vector<ChoiceItem>& items, ull& result,
                     const int style, const std::string& speaker,
                     const int notselected, const int selected)
    {
        result = 0;
        if (items.empty()) return;

        HideCursor();

        // -------- 无控制台回退 --------
        if (hConsole == INVALID_HANDLE_VALUE)
        {
            const auto P = PrepareChoices(items, "", style);
            for (const auto& e: P.entries)
            {
                if (e.kind == ChoiceKind::Blank)
                {
                    std::cout << '\n';
                    continue;
                }
                if (e.kind != ChoiceKind::Title) std::cout << e.prefix;
                std::cout << e.text << '\n';
            }

            const int max = static_cast<int>(P.highlightable.size());
            if (max == 0)
            {
                ShowCursor();
                return;
            }

            int input = 0;
            std::cout << "请选择 (输入序号): ";
            while (true)
            {
                while (!(std::cin >> input) or input < 1 or input > max)
                {
                    std::cin.clear();
                    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
                    std::cout << "无效输入，请重新输入: ";
                }
                if (const auto& e = P.entries[P.highlightable[input - 1]]; e.kind == ChoiceKind::Normal)
                {
                    result = static_cast<ull>(e.resultIdx);
                    break;
                }
                std::cout << "该选项不可选，请重新输入: ";
            }
            ShowCursor();
            return;
        }

        if (!speaker.empty())
        {
            SetConsoleTextAttribute(hConsole, COLOR_LBLUE);
            std::cout << speaker << ": " << std::flush;
            ResetConsoleColor_T();
        }

        const COORD startPos = GetConsoleCursorXY();
        const auto P         = PrepareChoices(items, speaker, style);

        const ll n = static_cast<ll>(P.highlightable.size());
        if (n == 0)
        {
            RenderChoices(P, startPos, -1, notselected, selected, style);
            ClearChoiceArea(startPos, P);
            SetConsoleCursorXY(0, startPos.Y);
            ShowCursor();
            return;
        }

        // ---------- 提示行 ----------
        constexpr std::string_view HINT_TEXT = "  [该项不可选]";
        const int HINT_WIDTH                 = static_cast<int>(DisplayWidth(HINT_TEXT));

        const COORD hintAnchor = {
            .X = 0,
            .Y = static_cast<SHORT>(P.layout.vertical
                                        ? startPos.Y + static_cast<int>(P.entries.size()) + 1
                                        : startPos.Y + 2)
        };

        auto hintPos = COORD{.X = -1, .Y = -1};

        auto ClearHint = [&]
        {
            if (hintPos.X < 0) return;
            SetConsoleCursorXY(hintPos.X, hintPos.Y);
            std::cout << std::string(HINT_WIDTH, ' ') << std::flush;
            hintPos = COORD{.X = -1, .Y = -1};
        };

        auto ShowHint = [&]
        {
            if (hintPos.X >= 0) return;
            SetConsoleCursorXY(hintAnchor.X, hintAnchor.Y);
            SetConsoleTextAttribute(hConsole, COLOR_LRED);
            std::cout << HINT_TEXT << std::flush;
            ResetConsoleColor_T();
            hintPos = hintAnchor;
        };

        // ---------- 主循环 ----------
        ll cursor       = 0;
        ll highlightIdx = static_cast<ll>(P.highlightable[0]);

        RenderChoices(P, startPos, highlightIdx, notselected, selected, style);

        while (true)
        {
            const int key = _getch();

            if (key == '\r')
            {
                if (const auto& cur = P.entries[P.highlightable[cursor]]; cur.kind == ChoiceKind::Normal)
                {
                    result = static_cast<ull>(cur.resultIdx);
                    break;
                }
                ShowHint();
                continue;
            }

            if (key != 224) continue;

            const int arrow = _getch();
            int dir;
            switch (arrow)
            {
                case 72:
                case 75: dir = -1;
                    break;
                case 80:
                case 77: dir = 1;
                    break;
                default: continue;
            }

            const ll newCursor = (cursor + dir + n) % n;
            if (newCursor != cursor) ClearHint();

            cursor       = newCursor;
            highlightIdx = static_cast<ll>(P.highlightable[cursor]);

            RenderChoices(P, startPos, highlightIdx, notselected, selected, style);
        }

        ClearHint();
        ClearChoiceArea(startPos, P);
        SetConsoleCursorXY(0, startPos.Y);
        ShowCursor();
    }
} // anonymous namespace


// ============================================================
// 公开入口
// ============================================================

// 结构化版本
void CChoiceEx(const std::vector<ChoiceItem>& choices, ull& result,
               const int style, const std::string& speaker,
               const int notselected, const int selected)
{
    CChoiceImpl(choices, result, style, speaker, notselected, selected);
}

// 字符串版本：剥离 <none>/<title>/<invalid> 前缀后转发
void CChoice(const std::vector<std::string>& choices, ull& result,
             const int style, const std::string& speaker,
             const int notselected, const int selected)
{
    std::vector<ChoiceItem> items;
    items.reserve(choices.size());
    for (const auto& s: choices)
    {
        ChoiceItem item;
        item.text = s;
        item.kind = StripChoicePrefix(item.text);
        items.push_back(std::move(item));
    }
    CChoiceImpl(items, result, style, speaker, notselected, selected);
}


// ============================================================
// 密码 / 输入框
// ============================================================
void CInputWhenCorrect(const std::string& prompt, const int hid,
                       const ull limit, const std::string& key,
                       const std::string& incor, const std::string& correct)
{
    const auto [X, Y] = GetConsoleCursorXY();
    std::string input;
    const PrintLn s;

    auto updateDisplay = [&]
    {
        SetConsoleCursorXY(X, Y);
        s.CPrint(prompt);
        if (hid == 1)
            s.CPrint(std::string(input.size(), '*')
                     + std::string(limit - input.size(), ' '));
        else
            s.CPrint(input + std::string(limit - input.size(), ' '));

        if (input.size() >= limit)
            s.CPrint("<lred> [ 上限" + std::to_string(limit) + "字符! ]</lred>");
        else
            s.CPrint(std::string(30, ' '));
    };

    while (true)
    {
        input.clear();
        while (true)
        {
            updateDisplay();
            const int ch = _getch();

            if (ch == 13) break;
            if (!input.empty() and ch == 8) input.pop_back();
            if (input.size() >= limit) continue;
            if (IsPrintableAscii(ch)) input.push_back(static_cast<char>(ch));
        }

        const size_t clearW = std::max(DisplayWidth(incor), DisplayWidth(correct));
        printf("\n%s\r", std::string(clearW, ' ').c_str());

        if (input == key)
        {
            s.CPrint(correct);
            break;
        }
        s.CPrint(incor);
    }
}

void CInputTo(const std::string& prompt, const int hid,
              const ull limit, std::string& ans)
{
    const auto [X, Y]    = GetConsoleCursorXY();
    const bool unlimited = limit == 0; // limit=0 → 不限制长度

    std::string input;
    const PrintLn s;

    // 上一次绘制写入的可见宽度，用于退格后覆盖尾部残留
    size_t lastW = 0;

    auto updateDisplay = [&]
    {
        SetConsoleCursorXY(X, Y);

        const std::string shown = (hid == 1)
                                      ? std::string(input.size(), '*')
                                      : input;

        // 只在有限模式下、且达到上限时显示提示
        const std::string hint =
                !unlimited && input.size() >= limit
                    ? "<lred> ^[ 上限" + std::to_string(limit) + "字符! ]</lred>"
                    : std::string{};

        const size_t promptW = DisplayWidth(prompt);
        const size_t curW    = promptW + input.size() + DisplayWidth(hint);

        // 绘制
        s.CPrint(prompt);
        s.CPrint(shown);
        if (!hint.empty()) s.CPrint(hint);

        // 本次比上次窄：用空格覆盖尾部残留（backspace 后清干净的关键）
        if (curW < lastW)
            std::cout << std::string(lastW - curW, ' ') << std::flush;

        lastW = std::max(lastW, curW);

        // 光标归位到输入末尾，方便继续输入
        SetConsoleCursorXY(X + static_cast<int>(promptW + input.size()), Y);
    };

    while (true)
    {
        updateDisplay();
        const int ch = getch();

        if (ch == 13) break;                               // 回车
        if (!input.empty() && ch == 8) input.pop_back();   // 退格
        if (!unlimited && input.size() >= limit) continue; // 超限，丢弃
        if (IsPrintableAscii(ch)) input.push_back(static_cast<char>(ch));
    }
    ans = input;
}

// ============================================================
// PrintLn::ReverseCPrint
// ============================================================
// 每个「可见字符」对应的列宽（跳过颜色标签与 ~）
// 注意：该函数假定内容是单行；多行场景（speaker 换行缩进）请避免使用
static std::vector<int> VisibleWidths(const std::string& content)
{
    std::vector<int> widths;
    size_t i = 0;
    while (i < content.size())
    {
        const char c = content[i];

        if (c == '~' || c == '\n')
        {
            ++i;
            continue;
        }

        if (c == '<')
        {
            if (const auto tag = TryParseColorTag(content, i); tag.valid)
            {
                i = tag.nextPos;
                continue;
            }
        }

        const int len = utf8CharLen(static_cast<unsigned char>(c));
        widths.push_back(len == 3 or len == 4 ? 2 : 1);
        i += len;
    }
    return widths;
}

void PrintLn::ReverseCPrint() const
{
    if (content.empty()) return;

    const auto widths = VisibleWidths(content);
    if (widths.empty()) return;

    // 无控制台：退化为「按可见字符数量清屏」
    if (hConsole == INVALID_HANDLE_VALUE)
    {
        PrintLn temp(*this);
        temp.delay = 0;
        temp.CPrint();
        for (size_t i = widths.size(); i-- > 0;)
        {
            std::cout << std::string(static_cast<size_t>(widths[i]), ' ') << std::flush;
            if (delay > 0) Sleep(delay);
        }
        return;
    }

    const auto [X, Y] = GetConsoleCursorXY();

    PrintLn temp(*this);
    temp.delay = 0;
    temp.CPrint();

    // speaker 前缀占用的列宽（"M1xTri: " 这样的形式 = size + 2）
    const int speakerWidth = speaker.empty()
                                 ? 0
                                 : static_cast<int>(speaker.size()) + 2;
    const int contentStartX = X + speakerWidth;

    // content 内部每个可见字符相对 content 起点的列偏移
    std::vector starts(widths.size() + 1, 0);
    for (size_t i = 0; i < widths.size(); ++i)
        starts[i + 1] = starts[i] + widths[i];

    // 从右向左，按可见字符的列宽精确定位覆盖
    for (size_t i = widths.size(); i-- > 0;)
    {
        SetConsoleCursorXY(contentStartX + starts[i], Y); // ← 加 speaker 偏移
        printf("%*s", widths[i], "");
        fflush(stdout);
        if (delay > 0) Sleep(delay);
    }
    SetConsoleCursorXY(X, Y);
}

void PrintLn::CPrint(const std::string& cont) const
{
    PrintLn s(*this);
    s.content = cont;
    s.CPrint();
}

void PrintLn::CPrint(const std::vector<std::string>& conts) const
{
    for (const auto& i: conts) CPrint(i);
}
