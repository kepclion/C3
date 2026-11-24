#pragma once
#include "allheader.h"
#include "ConsoleUtils.h"

constexpr std::string_view CHOICE_GAP = "   ";


// ============================================================
// CChoice 样式常量
// ============================================================
namespace ChoiceStyle
{
    // ---------- 方向 1 ----------
    inline constexpr uint32_t Horizontal = 0x00000000;
    inline constexpr uint32_t Vertical   = 0x10000000;

    // ---------- 编号 2 ----------
    inline constexpr uint32_t NumNone       = 0x00000000;
    inline constexpr uint32_t NumArabic     = 0x01000000;
    inline constexpr uint32_t NumRomanUpper = 0x02000000;
    inline constexpr uint32_t NumRomanLower = 0x03000000;

    // ---------- 装饰 3 ----------
    inline constexpr uint32_t NoDecor       = 0x00000000;
    inline constexpr uint32_t AngleOut      = 0x00100000; // > A <
    inline constexpr uint32_t AngleIn       = 0x00200000; // < A >
    inline constexpr uint32_t Bracket       = 0x00300000; // [ A ]
    inline constexpr uint32_t AngleOutRNone = 0x00400000; // > A

    // ---------- 掩码 ----------
    inline constexpr uint32_t DirectionMask = 0xF0000000;
    inline constexpr uint32_t NumMask       = 0x0F000000;
    inline constexpr uint32_t DecorMask     = 0x00F00000;

    // ---------- 常用组合 ----------
    inline constexpr uint32_t VNone = Vertical   | NumNone | NoDecor;
    inline constexpr uint32_t HNone = Horizontal | NumNone | NoDecor;

    inline constexpr uint32_t VBracket = Vertical   | NumNone | Bracket;
    inline constexpr uint32_t HBracket = Horizontal | NumNone | Bracket;

    inline constexpr uint32_t Default  = Vertical   | NumArabic | NoDecor;
    inline constexpr uint32_t Compact  = Horizontal | NumArabic | Bracket;
    inline constexpr uint32_t Menu     = Vertical   | NumArabic | Bracket;
    inline constexpr uint32_t Contract = Vertical   | NumRomanUpper | AngleOutRNone;
}

// ============================================================
// CChoice 条目类型
// ============================================================
enum class ChoiceKind : uint8_t
{
    Normal,     // 普通：编号 + 装饰，可高亮，可选中
    Blank,      // 空行：占一行，其他什么都不显示
    Title,      // 小标题：无编号、无装饰，不可高亮、不可选中
    Disabled,   // 禁用：有序号、可高亮，但 enter 无效
};

// 结构化条目（推荐用于新代码）
//   聚合体：可直接用 {text, kind} 初始化
struct ChoiceItem
{
    std::string text;
    ChoiceKind  kind = ChoiceKind::Normal;
};


// ============================================================
// 一段待打印的文本
// ============================================================
struct PrintLn
{
    std::string content;
    std::string speaker;
    int delay = 0;
    int hid   = 1;

    PrintLn() = default;

    PrintLn(const PrintLn&) = default;

    PrintLn& operator=(const PrintLn&) = default;

    void CPrint() const;

    void CPrint(const std::string& cont) const;

    void CPrint(const std::vector<std::string>& conts) const;

    void ReverseCPrint() const;
};

// ============================================================
// 动画效果（自由函数）
// ============================================================

struct CPrintThenBackParams
{
    PrintLn forward;
    std::string finalText;
    int delayForward = 20;
    int delayBack    = 5;
    int delayFinal   = 27;
    int waitForward  = 0;
    int waitBack     = 0;
};

// ---------- 布局 ----------
struct ChoiceLayout
{
    bool vertical = false;
    std::string indent;
    std::vector<std::string> prefixes;
    size_t prefixWidth     = 0;
    size_t decorWidth      = 0;
    size_t horizontalWidth = 0;
};

void CPrintThenBack(const CPrintThenBackParams& p);

void RollCPrint(const std::vector<std::string>& items,
                int windowLines,
                int blankLines,
                int delayMs,
                int loop);

void CirLoading(unsigned long long duration, std::string_view label);

void LineLoading(const char* header, unsigned long long total,
                 const char* filled = "*", const char* empty       = "-",
                 int type           = 0, unsigned long long length = 50,
                 int delayMs        = 50);

// ------------------------------------------------------------------
// CChoice  ——  基于前缀（推荐用法，调用点简洁）
//     "<none>..."    -> Blank
//     "<title>..."   -> Title
//     "<invalid>..." -> Disabled
//     无前缀         -> Normal
//
//   CChoice({
//       "<title>账户",
//       "登录",
//       "注册",
//       "<none>",
//       "<invalid>管理面板（未解锁）",
//       "退出"
//   }, resu, ChoiceStyle::Menu, "ENTER");
//
// CChoiceEx  ——  结构化（类型明确，便于动态构造）
//   CChoiceEx({
//       {"账户",     ChoiceKind::Title},
//       {"登录",     ChoiceKind::Normal},
//       {"注册",     ChoiceKind::Normal},
//       {"",         ChoiceKind::Blank},
//       {"管理面板",  ChoiceKind::Disabled},
//       {"退出",     ChoiceKind::Normal},
//   }, resu, ChoiceStyle::Menu, "ENTER");
//
// 两者 result 语义一致：仅 Normal 计入编号并返回。
// ------------------------------------------------------------------
void CChoice(const std::vector<std::string>& choices,
             unsigned long long& result,
             int style                  = ChoiceStyle::Default,
             const std::string& speaker = "",
             int notselected            = COLOR_LWHITE | COLOR_BLACK,
             int selected               = COLOR_LWHITE | BCOLOR_LGREEN);

void CChoiceEx(const std::vector<ChoiceItem>& choices,
               unsigned long long& result,
               int style                  = ChoiceStyle::Default,
               const std::string& speaker = "",
               int notselected            = COLOR_LWHITE | COLOR_BLACK,
               int selected               = COLOR_LWHITE | BCOLOR_LGREEN);

void CInputWhenCorrect(const std::string& prompt, int hid, unsigned long long limit,
                       const std::string& key,
                       const std::string& incor   = "<lred>密码错误</lred>\n",
                       const std::string& correct = "<lgreen>密码正确</lgreen>\n");

void CInputTo(const std::string& prompt, int hid,
              unsigned long long limit, std::string& ans);