#include "SaveData.h"
#include "allheader.h"
#include <cstdlib>

namespace PreludeIni
{
    constexpr auto kSection  = "progress";
    constexpr auto kVersion  = "version";
    constexpr auto kPrelude  = "prelude";
    constexpr auto kTasks    = "tasks";
    constexpr auto kLevel    = "level";
    constexpr auto kStoryId  = "storyid";
    constexpr auto kMixHD    = "MixHeartDenied";
    constexpr auto kToMidEnd = "ToMidEnd";
    constexpr auto kMidEnd   = "MidEnd";
}


//Prelude 枚举类互转
int toInt(const Prelude p) noexcept { return static_cast<int>(p); }

Prelude fromInt(const int v) noexcept
{
    switch (v)
    {
        case 0: return None;
        case 1: return Agreed;
        case 2: return LoggedIn;
        default: return None;
    }
}

int TaskProgress::doneCount() const noexcept
{
    int n = 0;
    for (int i = 0; i < kTaskCount; ++i) if (tasks_ & (1u << i)) ++n;
    return n;
}

namespace
{
    std::vector<std::string> SplitBy(const std::string& str, const char delim)
    {
        std::vector<std::string> out;
        if (str.empty()) return out;

        std::stringstream ss(str);
        std::string item;
        while (std::getline(ss, item, delim))
            if (!item.empty()) out.push_back(std::move(item));
        return out;
    }
}

SaveData SaveData::Load(const std::string& path)
{
    SaveData d;
    CSimpleIniA ini;
    ini.SetUnicode();
    if (ini.LoadFile(path.c_str()) < 0) return d;

    // 版本
    d.version_ = static_cast<int>(std::strtol(
        ini.GetValue(PreludeIni::kSection, PreludeIni::kVersion, "1"),
        nullptr, 10));

    // 序章
    const auto p = std::strtol(ini.GetValue(PreludeIni::kSection,
                                            PreludeIni::kPrelude,
                                            "0"), nullptr, 10);
    d.preludeProgress_.setPrelude(static_cast<int>(p));

    // 任务位掩码：以十六进制整数存
    d.taskProgress_.tasks_ = static_cast<uint32_t>(
        std::strtoull(ini.GetValue(PreludeIni::kSection, PreludeIni::kTasks, "0"),
                      nullptr, 0));

    // 等级
    d.levelProgress_.level_ = std::strtoull(
        ini.GetValue(PreludeIni::kSection, PreludeIni::kLevel, "0"),
        nullptr, 10);

    // 故事
    for (const auto& id: SplitBy(ini.GetValue(PreludeIni::kSection,
                                              PreludeIni::kStoryId, ""), '-'))
        d.storyProgress_.readStories_.insert(std::strtoull(id.c_str(), nullptr, 10));

    // 等级
    d.mixProgress_.HeartDenied = std::strtoull(
        ini.GetValue(PreludeIni::kSection, PreludeIni::kMixHD, "0"),
        nullptr, 10);

    //中点
    d.midEndProgress_.isMidEnd = std::strtoull(
        ini.GetValue(PreludeIni::kSection, PreludeIni::kMidEnd, "0"),
        nullptr, 10);
    d.midEndProgress_.isToMidEnd = std::strtoull(
        ini.GetValue(PreludeIni::kSection, PreludeIni::kToMidEnd, "0"),
        nullptr, 10);

    return d;
}

void SaveData::Save(const std::string& path) const
{
    CSimpleIniA ini;
    ini.SetUnicode();
    ini.LoadFile(path.c_str());

    // 版本
    ini.SetValue(PreludeIni::kSection,
                 PreludeIni::kVersion,
                 std::to_string(version_).c_str());

    // Prelude
    ini.SetValue(PreludeIni::kSection,
                 PreludeIni::kPrelude,
                 std::to_string(toInt(preludeProgress_.prelude_)).c_str());

    // Task —— 存 0x 前缀的十六进制
    char buf[16];
    std::snprintf(buf, sizeof(buf), "0x%X", taskProgress_.tasks_);
    ini.SetValue(PreludeIni::kSection, PreludeIni::kTasks, buf);

    // Level
    ini.SetValue(PreludeIni::kSection,
                 PreludeIni::kLevel,
                 std::to_string(levelProgress_.level_).c_str());

    // Story —— 用 '-' 分隔
    std::string str;
    for (const auto& id: storyProgress_.readStories_)
    {
        if (!str.empty()) str += '-';
        str += std::to_string(id);
    }
    ini.SetValue(PreludeIni::kSection, PreludeIni::kStoryId, str.c_str());

    // Mix
    ini.SetValue(PreludeIni::kSection,
                 PreludeIni::kMixHD,
                 std::to_string(mixProgress_.HeartDenied).c_str());

    // MidEnd
    ini.SetValue(PreludeIni::kSection,
                 PreludeIni::kToMidEnd,
                 std::to_string(midEndProgress_.isToMidEnd).c_str());
    ini.SetValue(PreludeIni::kSection,
                 PreludeIni::kMidEnd,
                 std::to_string(midEndProgress_.isMidEnd).c_str());

    (void) ini.SaveFile(path.c_str());
}
