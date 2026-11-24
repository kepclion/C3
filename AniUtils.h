#pragma once
#include <random>
#include <string>
#include <string_view>
#include <iostream>

#include "Animation.h"   // 依赖 ChoiceStyle 常量

// 显示宽度：跳过 <color>...</color> 颜色标签
size_t DisplayWidth(std::string_view s);

int utf8CharLen(unsigned char c);

std::mt19937& getRNG();

void typeUtf8Char(const std::string& utf8Char, int baseDelay);

double lerp(int type, double s);

bool isVertical(int style);

uint32_t decorOf(uint32_t style);

uint32_t numModeOf(uint32_t style);

char getLeftDecor(int style);

char getRightDecor(int style);

std::string getIndent(const std::string& speaker);

std::string toRoman(int n, bool upper);

bool IsPrintableAscii(int ch);