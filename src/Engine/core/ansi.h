/*************************
 * @file ansi.h
 * @author Zhu Zhile (zhuzhile08@gmail.com)
 * 
 * @brief ANSI escape color codes
 * 
 * @date 2022-02-19
 * 
 * @copyright Copyright (c) 2022
 *************************/

#pragma once

#include <iostream>
#include <string>

namespace lyra {

/**
 * @brief basic ANSI fonts
 */
typedef enum Font {
    NON = 0,
    BLD = 1,
    DIM = 2,
    ITC = 3,
    UNL = 4
} Font;

/**
 * @brief basic ANSI colors
 */
typedef enum Color {
    BLK = 30,
    RED = 31,
    GRN = 32,
    YEL = 33,
    BLU = 34,
    MAG = 35,
    CYN = 36,
    WHT = 97,
    GRY = 90,
    B_RED = 91,
    B_GRN = 92,
    B_YEL = 93,
    B_BLU = 94,
    B_MAG = 95,
    B_CYN = 96,
    DEF = 0
} Color;

/**
 * @brief generate a ANSI color code with a font and color
 * 
 * @param font font
 * @param color color
 */
inline void ANSI(Font font, Color color);

void ANSI(Font font, Color color) {
    std::cout << "\033[" << font << ";" << color << "m";
}

#define SET_COLOR_DEFAULT ANSI(NON, DEF);

} // namespace lyra