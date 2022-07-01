/***
 * @file log.h
 * @author Zhu Zhile (zhuzhile08@gmail.com)
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
#include <core/defines.h>
#include <core/settings.h>

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

	[[nodiscard]] static int font_cast(Font font) {
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
		DEF = 0
	};

	[[nodiscard]] static int color_cast(Color color) {
		return static_cast<int>(color);
	}

	static void ANSI(Font font, Color color) {
		std::cout << "\033[" << font_cast(font) << ";" << color_cast(color) << "m";
	}

	static void set_color_default() {
		ANSI(Font::NON, Color::DEF);
	}

public:
	[[nodiscard]] static const char* tab() {
		return "\t";
	}

	[[nodiscard]] static const char* end_l() {
		return "\n";
	}

	/**
	 * @brief log normal messages
	 *
	 * @tparam ...Args types to print
	 * @param ...message messages
	*/
	template<typename ... Args> static void log(Args... message) {
		// print the message
		(std::cout << ... << std::forward<Args>(message)) << end_l();
#ifdef LOG_FILE
		(_logFile << ... << std::forward<Args>(message)) << end_l();
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
#ifdef LOG_FILE
		_logFile << "[DEBUG]: ";
		(_logFile << ... << std::forward<Args>(message)) << end_l();
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
#ifdef LOG_FILE
		_logFile << "[INFO]: ";
		(_logFile << ... << std::forward<Args>(message)) << end_l();
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
#ifdef LOG_FILE
		_logFile << "[WARNING]: ";
		(_logFile << ... << std::forward<Args>(message)) << end_l();
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
#ifdef LOG_FILE
		_logFile << "[ERROR]: ";
		(_logFile << ... << std::forward<Args>(message)) << end_l();
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
#ifdef LOG_FILE
		_logFile << "[EXCEPTION]: ";
		(_logFile << ... << std::forward<Args>(message)) << end_l();
#endif
		// abourt the program
		std::abort();
		// reset color
		set_color_default();
	}

	/**
	 * @brief clear the termial buffer
	*/
	static void clear_buffer() {
		set_color_default();
		std::cout << std::endl;
	}

	~Logger() noexcept {
		_logFile.close();
	}

private:
	static std::ofstream _logFile;

	Logger() {
		std::ios::sync_with_stdio(Settings::Debug::stdio_sync);
		_logFile.open("data/log/log.txt", std::ofstream::out | std::ofstream::trunc); // because I kinda can't use the logger functionality in here, you just have to hope that this doesn't throw an error
	}
};

} // namespace lyra
