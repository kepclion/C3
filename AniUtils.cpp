#include "AniUtils.h"
#include "ConsoleUtils.h"   // getColorFromTag / INVALID_COLOR

//随机数生成器
std::mt19937& getRNG()
{
    static std::random_device rd;
    static std::mt19937 gen(rd());
    return gen;
}

//utf8字符长度
int utf8CharLen(const unsigned char c)
{
    if (c < 0x80) return 1;
    if ((c & 0xE0) == 0xC0) return 2;
    if ((c & 0xF0) == 0xE0) return 3;
    if ((c & 0xF8) == 0xF0) return 4;
    return 1;
}

//显示宽度（跳过 <color>...</color> 颜色标签）
size_t DisplayWidth(std::string_view s)
{
    size_t w = 0;
    size_t i = 0;
    while (i < s.size())
    {
        if (s[i] == '<')
        {
            if (const size_t close = s.find('>', i + 1); close != std::string_view::npos)
            {
                std::string_view tag = s.substr(i + 1, close - i - 1);
                if (!tag.empty() && tag.front() == '/') tag.remove_prefix(1);
                if (getColorFromTag(tag) != INVALID_COLOR)
                {
                    i = close + 1;
                    continue;
                }
            }
        }

        const auto c = static_cast<unsigned char>(s[i]);
        if (c < 0x80)                 { w += 1; i += 1; }
        else if ((c & 0xE0) == 0xC0) { w += 1; i += 2; }
        else if ((c & 0xF0) == 0xE0) { w += 2; i += 3; }
        else if ((c & 0xF8) == 0xF0) { w += 2; i += 4; }
        else                         { w += 1; i += 1; }
    }
    return w;
}

//打出字符
void typeUtf8Char(const std::string& utf8Char, const int baseDelay)
{
    static std::uniform_real_distribution dist(1.0, 2.0);
    static auto& gen = getRNG();

    std::cout << utf8Char << std::flush;

    if (utf8Char == " ") Sleep(baseDelay);
    Sleep(static_cast<int>(baseDelay * dist(gen)));
}

//插值
double lerp(const int type, const double s)
{
    constexpr double PI = 3.141592653589793;
    switch (type)
    {
        case 11: return s * s;
        case 12: return 1 - std::sqrt(1 - s * s);
        case 13: return s / (2 - s);
        case 21: return s * (2 - s);
        case 22: return 2 * s / (1 + s);
        case 23: return std::sqrt(s);
        case 24: return std::sqrt(s) * (2 - std::sqrt(s));
        case 31: return s + std::sin(6 * s * PI) / (6 * PI);
        default: return s;
    }
}


// ============================================================
// CChoice 相关
// ============================================================
bool isVertical(const int style)
{
    return (style & ChoiceStyle::DirectionMask) == ChoiceStyle::Vertical;
}

uint32_t decorOf(const uint32_t style)
{
    return style & ChoiceStyle::DecorMask;
}

uint32_t numModeOf(const uint32_t style)
{
    return style & ChoiceStyle::NumMask;
}

char getLeftDecor(const int style)
{
    switch (decorOf(style))
    {
        case ChoiceStyle::AngleOutRNone:
        case ChoiceStyle::AngleOut: return '>';
        case ChoiceStyle::AngleIn:  return '<';
        case ChoiceStyle::Bracket:  return '[';
        default: return '\0';
    }
}

char getRightDecor(const int style)
{
    switch (decorOf(style))
    {
        case ChoiceStyle::AngleOut: return '<';
        case ChoiceStyle::AngleIn:  return '>';
        case ChoiceStyle::Bracket:  return ']';
        case ChoiceStyle::AngleOutRNone:
        default: return '\0';
    }
}

std::string getIndent(const std::string& speaker)
{
    if (speaker.empty()) return "";
    return std::string(speaker.size() + 2, ' ');
}


const char* const ROMAN_SMALL[] =
{
    "", "I", "II", "III", "IV", "V", "VI", "VII", "VIII", "IX", "X"
};

std::string toRoman(int n, const bool upper)
{
    std::string s;
    if (n >= 1 && n <= 10)
    {
        s = ROMAN_SMALL[n];
    }
    else
    {
        static const std::pair<int, const char*> table[] = {
            {1000, "M"}, {900, "CM"}, {500, "D"}, {400, "CD"},
            {100, "C"}, {90, "XC"}, {50, "L"}, {40, "XL"},
            {10, "X"}, {9, "IX"}, {5, "V"}, {4, "IV"}, {1, "I"}
        };
        for (const auto& [v, sym] : table)
            while (n >= v)
            {
                s += sym;
                n -= v;
            }
    }
    if (!upper)
        for (char& c : s)
            c = static_cast<char>(std::tolower(static_cast<unsigned char>(c)));
    return s;
}

bool IsPrintableAscii(const int ch)
{
    return 32 <= ch and ch < 127;
}