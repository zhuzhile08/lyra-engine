/*************************
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

#ifdef _WIN32
#include <windows.h>
#endif

#include <iostream>
#include <string>
#include <fstream>
#include <utility>

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
		(std::cout << ... << std::forward<Args>(message)) << end_l();
#ifdef LYRA_LOG_FILE
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
		(std::cout << ... << std::forward<Args>(message)) << end_l();
#ifdef LYRA_LOG_FILE
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
		(std::cout << ... << std::forward<Args>(message)) << end_l();
#ifdef LYRA_LOG_FILE
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
		(std::cout << ... << std::forward<Args>(message)) << end_l();
#ifdef LYRA_LOG_FILE
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
		(std::cout << ... << std::forward<Args>(message)) << end_l();
#ifdef LYRA_LOG_FILE
		(m_logFile << ... << std::forward<Args>(message)) << end_l();
#endif
		// abourt the program
		std::abort();
		// reset color
		set_color_default();
	}

	template<typename Answer, typename ... Args> static void ask(Answer& answer, Args... message) {
		// set the color and font of the message
		ANSI(Font::BLD, Color::WHT);
		// print the message
		(std::cout << ... << std::forward<Args>(message));
		std::getline(std::cin, answer);
		// reset color
		set_color_default();
	}

	/**
	 * @brief clear the termial buffer
	*/
	static void clear_buffer();

	/**
	 * @brief return tab character
	 * 
	 * @return const* 
	 */
	NODISCARD static const char* tab() {
		return "\t";
	}
	/**
	 * @brief return new line character
	 * 
	 * @return const* 
	 */
	NODISCARD static const char* end_l() {
		return "\n";
	}

private:
	static std::ofstream m_logFile;

	Logger() noexcept = delete;
};

template<typename ... Args> static void lassert(bool condition, Args... message) {
#ifndef NDEBUG
	if (!condition) (Logger::log_exception(message), ...);
#else
	if (condition); // cused, I know, but I'm waaaay too lazy to put a debug check on every assert
#endif
}