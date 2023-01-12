/***
 * @file log.h
 * @author zhuzhile08 (zhuzhile08@gmail.com)
 *
 * @brief header with loggers
 * @brief function-esque macros for debugging
 * @brief logs messages and exceptions with coloring
 * @brief ANSI color codes
 *
 * @date 2022-02-03
 *
 * @copyright Copyright (c) 2022
 *
 *************************/

#pragma once

#include <iostream>
#include <fstream>
#include <utility>

#include <vulkan/vulkan.h>

namespace lyra {

#define NODISCARD [[nodiscard]]

class Logger {
private:
	/**
	 * @brief basic ANSI fonts
	 */
	enum class Font : int {
		NON = 0,
		BLD = 1,
		DIM = 2,
		ITC = 3,
		UNL = 4
	};

	/**
	 * @brief cast a font enum value to an integer
	 * 
	 * @param font font to cast
	 * 
	 * @return constexpr int
	 */
	NODISCARD constexpr int font_cast(Font font) {
		return static_cast<int>(font);
	}

	/**
	 * @brief basic ANSI colors
	 */
	enum class Color : int {
		BLK = 30,
		RED = 31,
		GRN = 32,
		YEL = 33,
		BLU = 34,
		MAG = 35,
		CYN = 36,
		GRY = 90,
		B_RED = 91,
		B_GRN = 92,
		B_YEL = 93,
		B_BLU = 94,
		B_MAG = 95,
		B_CYN = 96,
		WHT = 97,
		DEF = WHT
	};

	/**
	 * @brief cast a color enum value to an integer
	 * 
	 * @param color color to cast
	 * 
	 * @return constexpr int
	 */
	NODISCARD constexpr int color_cast(Color color) {
		return static_cast<int>(color);
	}

	/**
	 * @brief print an ANSI escape code to set font and color
	 * 
	 * @param font font
	 * @param color color
	 */
	void ANSI(Font font, Color color) {
		std::cout << "\033[" << font_cast(font) << ";" << color_cast(color) << "m";
	}
	/**
	 * @brief set the color to default
	 */
	void set_color_default() {
		ANSI(Font::NON, Color::WHT);
	}

public:
	/**
	 * @brief log normal messages
	 *
	 * @tparam ...Args types to print
	 * @param ...message messages
	*/
	template <typename ... Args> void log(Args... message) {
#ifndef NDEBUG
		// print the message
		(std::cout << ... << std::forward<Args>(message)) << end_l();
#ifdef LYRA_LOG_FILE
		(m_logFile << ... << std::forward<Args>(message)) << end_l();
#endif
#endif
	}

	/**
	 * @brief log debug messages
	 *
	 * @tparam ...Args types to print
	 * @param ...message messages
	*/
	template <typename ... Args> void debug(Args... message) {
#ifndef NDEBUG
		// set the color and font of the message
		ANSI(Font::NON, Color::GRY);
		// print the message
		std::cout << "[DEBUG]: ";
		(std::cout << ... << std::forward<Args>(message)) << end_l();
#ifdef LYRA_LOG_FILE
		m_logFile << "[DEBUG]: ";
		(m_logFile << ... << std::forward<Args>(message)) << end_l();
#endif
		// reset color
		set_color_default();
#endif
	}

	/**
	 * @brief log informational messages
	 *
	 * @tparam ...Args types to print
	 * @param ...message messages
	*/
	template <typename ... Args> void info(Args... message) {
#ifndef NDEBUG
		// set the color and font of the message
		ANSI(Font::NON, Color::GRN);
		// print the message
		std::cout << "[INFO]: ";
		(std::cout << ... << std::forward<Args>(message)) << end_l();
#ifdef LYRA_LOG_FILE
		m_logFile << "[INFO]: ";
		(m_logFile << ... << std::forward<Args>(message)) << end_l();
#endif
		// reset color
		set_color_default();
#endif
	}

	/**
	 * @brief log warnings
	 *
	 * @tparam ...Args types to print
	 * @param ...message messages
	*/
	template <typename ... Args> void warning(Args... message) {
#ifndef NDEBUG
		// set the color and font of the message
		ANSI(Font::NON, Color::YEL);
		// print the message
		std::cout << "[WARNING]: ";
		(std::cout << ... << std::forward<Args>(message)) << end_l();
#ifdef LYRA_LOG_FILE
		m_logFile << "[WARNING]: ";
		(m_logFile << ... << std::forward<Args>(message)) << end_l();
#endif
		// reset color
		set_color_default();
#endif
	}

	/**
	 * @brief log errors
	 *
	 * @tparam ...Args types to print
	 * @param ...message messages
	*/
	template <typename ... Args> void error(Args... message) {
		// set the color and font of the message
		ANSI(Font::NON, Color::RED);
		// print the message
		std::cout << "[ERROR]: ";
		(std::cout << ... << std::forward<Args>(message)) << end_l();
#ifdef LYRA_LOG_FILE
		m_logFile << "[ERROR]: ";
		(m_logFile << ... << std::forward<Args>(message)) << end_l();
#endif
		// reset color
		set_color_default();
	}

	/**
	 * @brief log exceptions
	 *
	 * @tparam ...Args types to print
	 * @param ...message messages
	*/
	template <typename ... Args> void exception(Args... message) {
		// set the color and font of the message
		ANSI(Font::BLD, Color::RED);
		// print the message
		std::cout << "[EXCEPTION]: ";
		(std::cerr << ... << std::forward<Args>(message)) << end_l();
#ifdef LYRA_LOG_FILE
		m_logFile << "[EXCEPTION]: ";
		(m_logFile << ... << std::forward<Args>(message)) << end_l();
#endif
		// reset color
		set_color_default();
	}

	/**
	 * @brief clear the termial buffer
	*/
	void clear_buffer();

	/**
	 * @brief tab escape character
	 * 
	 * @return const char*
	 */
	NODISCARD const char* tab() {
		return "\t";
	}
	/**
	 * @brief line end escape character
	 * 
	 * @return const char*
	 */
	NODISCARD const char* end_l() {
		return "\n";
	}

private:
	std::ofstream m_logFile;

	Logger();

	friend Logger& log();
};

/**
 * @brief return a static instance of the Logger
 * 
 * @return Logger& 
 */
Logger& log();

/**
 * @brief custom assert function to check if a condition is false
 * 
 * @tparam Args variadic message template
 * @param condition condition to check if false
 * @param message exception message
 */
template <typename ... Args> void lassert(bool condition, Args... message) {
	if (!condition) (log().exception(message), ...);
}
/**
 * @brief Vulkan function assert
 * 
 * @tparam Args variadic message template
 * @param function Vulkan function to check
 * @param purpose Purpose of the function
 */
template <typename Arg> void vassert(VkResult function, Arg purpose) {
	if (function != VkResult::VK_SUCCESS) (log().exception("Failed to ", purpose, " with error code: ", function, "!"));
}

} // namespace lyra
