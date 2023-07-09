/*************************
 * @file Logger.h
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

#include <Common/Common.h>

#include <iostream>
#include <fstream>
#include <utility>

#include <vulkan/vulkan.h>

namespace lyra {

class Logger {
private:
	enum class Font {
		non = 0,
		bold = 1,
		dim = 2,
		italic = 3,
		underline = 4,
		blink = 5
	};

	enum class Color {
		black = 30,
		red = 31,
		green = 32,
		yellow = 33,
		blue = 34,
		magenta = 35,
		cyan = 36,
		grey = 90,
		gray = grey,
		backgroundRed = 91,
		backgroundGreen = 92,
		backgroundYellow = 93,
		backgroundBlue = 94,
		backgroundMagenta = 95,
		backgroundCyan = 96,
		white = 97
	};

	NODISCARD constexpr int color_cast(Color color) {
		return static_cast<int>(color);
	}
	NODISCARD constexpr int font_cast(Font font) {
		return static_cast<int>(font);
	}

	void ansi(Font font, Color color) {
		std::cout << "\033[" << font_cast(font) << ";" << color_cast(color) << "m";
	}
	/**
	 * @brief set the color to default
	 */
	void set_color_default() {
		ansi(Font::non, Color::white);
	}

public:
	template <typename ... Args> constexpr void message(Args... message) {
#ifndef NDEBUG
		// print the message
		(std::cout << ... << std::forward<Args>(message)) << end_l();
#ifdef LYRA_LOG_FILE
		(m_logFile << ... << std::forward<Args>(message)) << end_l();
#endif
#endif
	}

	template <typename ... Args> constexpr void debug(Args... message) {
#ifndef NDEBUG
		// set the color and font of the message
		ansi(Font::non, Color::grey);
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

	template <typename ... Args> constexpr void info(Args... message) {
#ifndef NDEBUG
		// set the color and font of the message
		ansi(Font::non, Color::green);
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

	template <typename ... Args> constexpr void warning(Args... message) {
#ifndef NDEBUG
		// set the color and font of the message
		ansi(Font::non, Color::yellow);
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

	template <typename ... Args> constexpr void error(Args... message) {
		// set the color and font of the message
		ansi(Font::non, Color::red);
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

	template <typename ... Args> constexpr void exception(Args... message) {
		// set the color and font of the message
		ansi(Font::bold, Color::red);
		// print the message
		std::cout << "[EXCEPTION]: ";
		(std::cout << ... << std::forward<Args>(message)) << end_l();
#ifdef LYRA_LOG_FILE
		m_logFile << "[EXCEPTION]: ";
		(m_logFile << ... << std::forward<Args>(message)) << end_l();
#endif
		throw std::runtime_error("lyra::log()::exception(): An exception occured!");
		// reset color
		set_color_default();
	}

	void clear_buffer();

	NODISCARD constexpr const char* const tab() {
		return "\t";
	}
	NODISCARD constexpr const char* const end_l() {
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
template <typename ... Args> constexpr void lassert(bool condition, Args... message) {
	if (!condition) (log().exception(message), ...);
}
/**
 * @brief Vulkan function assert
 * 
 * @tparam Args variadic message template
 * @param function Vulkan function to check
 * @param purpose Purpose of the function
 */
template <typename Arg> constexpr void vassert(VkResult function, Arg purpose) {
	if (function != VkResult::VK_SUCCESS) (log().exception("Vulkan Exception: Failed to ", purpose, " with error code: ", function, "!"));
}

} // namespace lyra
