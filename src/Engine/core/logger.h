/***
 * @file log.h
 * @author Zhu Zhile (zhuzhile08@gmail.com)
 * 
 * @brief header with loggers
 * @brief function-esque macros for debugging
 * @brief logs messages and exeptions with coloring
 * @brief ANSI color codes
 * 
 * @date 2022-02-03
 * 
 * @copyright Copyright (c) 2022
 * 
 *************************/

#pragma once

#include <iostream>
#include <utility>
#include <core/ansi.h>
#include <core/defines.h>

namespace lyra {

// tab
#define TAB     std::string("\t")
// end line
#define END_L   std::string("\n")

// define which logs will be shown
#define ENABLE_DEBUG_LOG  1
#define ENABLE_INFO_LOG   1
#define ENABLE_NORMAL_LOG 1
#define ENABLE_WARN_LOG   1

#ifdef ndebug		// everything exept exeptions and errors are not printed on default when the programm is in release mode
#define ENABLE_DEBUG_LOG  0
#define ENABLE_INFO_LOG   0
#define ENABLE_NORMAL_LOG 0
#define ENABLE_WARN_LOG   0
#endif

/**
 * @brief fatality level of a message
 */
enum class LogLevel : int {
	LEVEL_DEBUG = 0,
	LEVEL_INFO = 1,
	LEVEL_WARNING = 2,
	LEVEL_ERROR = 3,
	LEVEL_FATAL = 4,
	LEVEL_NONE = 5
};

inline void log_message();

void log_message() {
	std::cout << END_L;
}

/**
 * @brief logs simple messages with colors. Can also print exeptions.
 * 
 * @param message the message
 * @param fatality the level of fatality of the message
 * @param tabNum how many abs to put before the message
 * @param ... the rest of the message
 */
template<typename ... Args> inline void log_message(const LogLevel fatality, Args ... message);

template<typename ... Args> void log_message(const LogLevel fatality, Args... message) {
    // fatality of a log
    std::string level[6] = {
        "[DEBUG]: ", 
        "[INFO]: ", 
        "[WARNING]: ", 
        "[ERROR]: ", 
        "[FATAL]: ", 
        ""
    };

    switch(fatality) {
    case(LogLevel::LEVEL_DEBUG)   : ANSI(Font::NON, Color::GRY); break;
    case(LogLevel::LEVEL_INFO)    : ANSI(Font::NON, Color::GRN); break;
    case(LogLevel::LEVEL_WARNING) : ANSI(Font::NON, Color::YEL); break;
    case(LogLevel::LEVEL_ERROR)   : ANSI(Font::NON, Color::RED); break;
    case(LogLevel::LEVEL_FATAL)   : ANSI(Font::BLD, Color::RED); break;
    }

    // print the fatality
    std::cout << level[static_cast<int>(fatality)];
    // print the message
	(fatality == LogLevel::LEVEL_FATAL) ? ((std::cerr << std::forward<Args>(message)), ...) : ((std::cout << std::forward<Args>(message)), ...);
    // reset colors and make a new linje
    std::cout << END_L;
    set_color_default();
}

/**
 * @brief clear the termial buffer
 */
inline void clear_terminal_buffer();

void clear_terminal_buffer() {
    set_color_default();
    std::cout << std::endl;
}

// debug functions
#define TEST_REACHED 			log_message(LogLevel::LEVEL_DEBUG, "reached")

#define LOG_ERROR(msg, ...) 	log_message(LogLevel::LEVEL_ERROR, msg, ##__VA_ARGS__)
#define LOG_EXEPTION(msg, ...) 	log_message(LogLevel::LEVEL_FATAL, msg, ##__VA_ARGS__)

#if ENABLE_WARN_LOG == 1
#define LOG_WARNING(msg, ...) 	log_message(LogLevel::LEVEL_WARNING, msg, ##__VA_ARGS__)
#endif

#if ENABLE_INFO_LOG == 1
#define LOG_INFO(msg, ...) 		log_message(LogLevel::LEVEL_INFO, msg, ##__VA_ARGS__)
#endif

#if ENABLE_DEBUG_LOG == 1
#define LOG_DEBUG(msg, ...) 	log_message(LogLevel::LEVEL_DEBUG, msg, ##__VA_ARGS__)
#endif

#if ENABLE_INFO_LOG == 1
#define LOG_INFO(msg, ...) 	log_message(LogLevel::LEVEL_INFO, msg, ##__VA_ARGS__)
#endif

};
