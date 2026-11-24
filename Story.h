#pragma once
#include <nlohmann/json.hpp>
#include <string>

// 首次启动时创建带默认值的 config.ini；已存在则跳过。
// 应在 main() 开头调用一次。
void InitConfig(const std::string& file = "config.ini");

void InitializeStory();

void ProcessStory(const nlohmann::basic_json<>& lineslist);

// 若光标已贴到窗口底部，会提示「按任意键继续」并清屏；
// 返回 true 表示刚刚翻过页。
// 供手写剧情（Line.cpp）在长段落之间调用。
bool EnsureRoomForNewPage();