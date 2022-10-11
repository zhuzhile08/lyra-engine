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

#include <core/decl.h>

namespace lyra {

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
	 */
	NODISCARD static int font_cast(Font font) {
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
		WHT = 97,
		GRY = 90,
		B_RED = 91,
		B_GRN = 92,
		B_YEL = 93,
		B_BLU = 94,
		B_MAG = 95,
		B_CYN = 96,
		DEF = WHT
	};

	/**
	 * @brief cast a color enum value to an integer
	 * 
	 * @param color color to cast
	 */
	NODISCARD static int color_cast(Color color) {
		return static_cast<int>(color);
	}

	/**
	 * @brief print an ANSI escape code to set font and color
	 * 
	 * @param font font
	 * @param color color
	 */
	static void ANSI(Font font, Color color) {
		std::cout << "\033[" << font_cast(font) << ";" << color_cast(color) << "m";
	}
	/**
	 * @brief set the color to default
	 */
	static void set_color_default() {
		ANSI(Font::NON, Color::WHT);
	}

public:
	/**
	 * @brief log normal messages
	 *
	 * @tparam ...Args types to print
	 * @param ...message messages
	*/
	template<typename ... Args> static void log(Args... message) {
		// print the message
		(std::cout << ... << std::forward<Args>(message)) << end_l();
#ifdef LYRA_LOG_FILE
		(m_logFile << ... << std::forward<Args>(message)) << end_l();
#endif
	}

	/**
	 * @brief log debug messages
	 *
	 * @tparam ...Args types to print
	 * @param ...message messages
	*/
	template<typename ... Args> static void log_debug(Args... message) {
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
	}

	/**
	 * @brief log informational messages
	 *
	 * @tparam ...Args types to print
	 * @param ...message messages
	*/
	template<typename ... Args> static void log_info(Args... message) {
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
	}

	/**
	 * @brief log warnings
	 *
	 * @tparam ...Args types to print
	 * @param ...message messages
	*/
	template<typename ... Args> static void log_warning(Args... message) {
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
	}

	/**
	 * @brief log errors
	 *
	 * @tparam ...Args types to print
	 * @param ...message messages
	*/
	template<typename ... Args> static void log_error(Args... message) {
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
	template<typename ... Args> static void log_exception(Args... message) {
		// set the color and font of the message
		ANSI(Font::BLD, Color::RED);
		// print the message
		std::cout << "[EXCEPTION]: ";
		(std::cout << ... << std::forward<Args>(message)) << end_l();
#ifdef LYRA_LOG_FILE
		m_logFile << "[EXCEPTION]: ";
		(m_logFile << ... << std::forward<Args>(message)) << end_l();
#endif
		// reset color
		set_color_default();
		// abourt the program
		std::abort();
	}

	/**
	 * @brief clear the termial buffer
	*/
	static void clear_buffer();

	/**
	 * @brief tab escape character
	 * 
	 * @return const char*
	 */
	NODISCARD static const char* tab() {
		return "\t";
	}
	/**
	 * @brief line end escape character
	 * 
	 * @return const char*
	 */
	NODISCARD static const char* end_l() {
		return "\n";
	}

private:
	static std::ofstream m_logFile;

	Logger() noexcept = delete;
};

/**
 * @brief custom assert function to check if a condition is false
 * 
 * @tparam Args variadic message template
 * @param condition condition to check if false
 * @param message exception message
 */
template<typename ... Args> static void lassert(bool condition, Args... message) {
#ifndef NDEBUG
	if (!condition) (Logger::log_exception(message), ...);
#else
	if (condition) // cused, I know, but I'm waaaay too lazy to put a debug check on every assert
	;
#endif
}
/**
 * @brief Vulkan function assert
 * 
 * @tparam Args variadic message template
 * @param function Vulkan function to check
 * @param purpose Purpose of the function
 */
template<typename Arg> static void vassert(VkResult function, Arg purpose) {
#ifndef NDEBUG
	if (function != VkResult::VK_SUCCESS) (Logger::log_exception("Failed to ", purpose, " with error code: ", function, "!"));
#else
	if (function) // cused, I know, but I'm waaaay too lazy to put a debug check on every assert
	;
#endif
}

} // namespace lyra
