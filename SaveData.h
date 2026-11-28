// SaveData.h
#pragma once
#include <set>
#include <string>
#include "allheader.h"

// 当前存档格式版本；每次字段含义/编码变更时 +1
inline constexpr int kSaveVersion = 1;
inline constexpr int kTaskCount   = Count;

struct PreludeProgress
{
    Prelude prelude_ = None;

    void setPrelude(const auto v) noexcept { prelude_ = static_cast<Prelude>(v); }
};

struct TaskProgress
{
    uint32_t tasks_ = 0;

    void markTaskDone(const Tasks t) noexcept { tasks_ |= bitOf(t); }
    void markTaskUndone(const Tasks t) noexcept { tasks_ &= ~bitOf(t); }

    [[nodiscard]] int doneCount() const noexcept;

    [[nodiscard]] bool isTaskDone(const Tasks t) const noexcept { return (tasks_ & bitOf(t)) != 0; }

    [[nodiscard]] bool allTasksDone() const noexcept { return doneCount() == kTaskCount; }

    static constexpr uint32_t bitOf(const Tasks t) noexcept { return 1u << static_cast<uint32_t>(t); }
};

struct LevelProgress
{
    ull level_ = 0;

    void setLevel(const ull level) noexcept { level_ = level; }
};

struct StoryProgress
{
    std::set<ull> readStories_;

    [[nodiscard]]
    bool isStoryRead(const ull& id) const { return readStories_.contains(id); }

    void markStoryRead(const ull& id) { readStories_.insert(id); }
};

struct MixProgress
{
    ull HeartDenied = 0;
};

struct MidEndProgress
{
    ull isToMidEnd = 0;
    ull isMidEnd = 0;
};



struct SaveData
{
    int version_ = kSaveVersion;
    PreludeProgress preludeProgress_;
    StoryProgress storyProgress_;
    TaskProgress taskProgress_;
    LevelProgress levelProgress_;
    MixProgress mixProgress_;
    MidEndProgress midEndProgress_;

    void Save(const std::string& path = "config.ini") const;

    static SaveData Load(const std::string& path = "config.ini");
};

extern SaveData save;
extern GameSurface SURFACE;


struct Task
{
    Tasks taskname;
    std::string mail    = "<grey>None</grey>";
    std::string message = "<grey>None</grey>";

    void taskDone() const { save.taskProgress_.markTaskDone(taskname); }
};


// 游戏内虚构邮箱与任务，不代表真实个人或现实事件。
inline const std::vector<Task> MailboxAndTask =
{
    {
        .taskname = GameCheat,
        .mail     = "kapacity@latte.loup",
        .message  = "1844-6744-0737-0955-1616",
    },
    {
        .taskname = DecodeZip,
        .mail     = "skewer@clydog.hsk",
        .message  = "Kepler"
    },
    {
        .taskname = FindQQClue,
        .mail     = "loupmauvais@haki.tbh",
        .message  = "1227280205"
    },
    {
        .taskname = DocumentFault,
        .mail     = "19114514@stu.eju.cod",
        .message  = "MATCH"
    },
    {
        .taskname = HeartKiller,
        //.mail    =,
        //.message  =
    },
};
