#pragma once
#include "../SaveData.h"

void prelude_intro();

// 内部调用 exit(0)，标记 [[noreturn]] 便于编译器优化与静态分析
[[noreturn]]
void prelude_intro_refuse();
void prelude_intro_agree(SaveData& saves);

void KRPhistory_0(SaveData& saves);
void KRPhistory_1(SaveData& saves);
void KRPhistory_2(SaveData& saves);
void KRPhistory_3(SaveData& saves);
void KRPhistory_4(SaveData& saves);


