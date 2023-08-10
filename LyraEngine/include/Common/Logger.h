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
#include <Common/Utility.h>
#include <Common/Config.h>
#include <Common/Array.h>
#include <Common/FileSystem.h>

#include <fmt/core.h>
#include <fmt/chrono.h>
#include <vulkan/vulkan.h>

#include <utility>
#include <mutex>
#include <chrono>

template <class Type> struct fmt::formatter<Type, std::enable_if_t<std::is_enum<Type>::value, char>> : 
	fmt::formatter<int> {
	auto format(const Type& value, fmt::format_context& context) {
		return fmt::formatter<int>::format(static_cast<int>(value), context);
	}
};

namespace lyra {

namespace ansi {

enum class Font {
	none,
	reset = none,
	bold,
	dim,
	italic,
	underline,
	blink
};

inline constexpr std::string set_style(Font font, uint32 foreground, uint32 background) {
	if constexpr (config::coloredLog == true)
		return fmt::format("\033[{};38;5;{};48;5;{}m", static_cast<int>(font), foreground, background);
	else return "";
}
inline constexpr std::string set_style(Font font, uint32 foreground) {
	if constexpr (config::coloredLog == true)
		return fmt::format("\033[{};38;5;{}m", static_cast<int>(font), foreground);
	else return "";
}
inline constexpr std::string reset_style() noexcept {
	if constexpr (config::coloredLog == true)
		return "\033[0;0;0m";	
	else return "";
}

} // namespace ansi

namespace log {

namespace detail {

void add_logger(const Logger& logger);
Logger* const default_logger();

} // namespace detail

enum class Level {
	log,
	trace,
	debug,
	info,
	warning,
	error,
	exception
};

class Logger {
public:
	Logger() = default;
	Logger(FILE* out, FILE* err, std::string_view name) : m_outStream(out), m_errStream(err), m_name(name) { detail::add_logger(*this); }
	Logger(FILE* stream, std::string_view name) : m_outStream(stream), m_errStream(stream), m_name(name) { detail::add_logger(*this); }

	template <class Format, typename ... Args> constexpr void log(Format&& format, Args&&... message) {
		fmt::print(m_outStream, fmt::runtime(std::forward<Format>(format)), std::forward<Args>(message)...);
	}
	template <class Format, typename ... Args> constexpr void trace(Format&& format, Args&&... message) {
		log<Level::trace>(std::forward<Format>(format), std::forward<Args>(message)...);
	}
	template <class Format, typename ... Args> constexpr void debug(Format&& format, Args&&... message) {
		log<Level::debug>(std::forward<Format>(format), std::forward<Args>(message)...);
	}
	template <class Format, typename ... Args> constexpr void info(Format&& format, Args&&... message) {
		log<Level::info>(std::forward<Format>(format), std::forward<Args>(message)...);
	}
	template <class Format, typename ... Args> constexpr void warning(Format&& format, Args&&... message) {
		log<Level::warning>(std::forward<Format>(format), std::forward<Args>(message)...);
	}
	template <class Format, typename ... Args> constexpr void error(Format&& format, Args&&... message) {
		log<Level::error>(std::forward<Format>(format), std::forward<Args>(message)...);
	}
	template <class Format, typename ... Args> constexpr void exception(Format&& format, Args&&... message) {
		log<Level::exception>(std::forward<Format>(format), std::forward<Args>(message)...);
	}

	template <class Msg> constexpr void log(Msg&& message) {
		fmt::print(m_outStream, std::forward<Msg>(message));
	}
	template <class Msg> constexpr void trace(Msg&& message) {
		log<Level::trace>(std::forward<Msg>(message));
	}
	template <class Msg> constexpr void debug(Msg&& message) {
		log<Level::debug>(std::forward<Msg>(message));
	}
	template <class Msg> constexpr void info(Msg&& message) {
		log<Level::info>(std::forward<Msg>(message));
	}
	template <class Msg> constexpr void warning(Msg&& message) {
		log<Level::warning>(std::forward<Msg>(message));
	}
	template <class Msg> constexpr void error(Msg&& message) {
		log<Level::error>(std::forward<Msg>(message));
	}
	template <class Msg> constexpr void exception(Msg&& message) {
		log<Level::exception>(std::forward<Msg>(message));
	}

	constexpr void new_line() {
		fmt::print(m_outStream, "\n");
	}
	
	NODISCARD constexpr const std::FILE* const outStream() const noexcept {
		return m_outStream;
	}
	NODISCARD constexpr const std::FILE* const errStream() const noexcept {
		return m_errStream;
	}
	NODISCARD constexpr std::FILE* outStream() noexcept {
		return m_outStream;
	}
	NODISCARD constexpr std::FILE* errStream() noexcept {
		return m_errStream;
	}
	NODISCARD constexpr std::string name() const noexcept {
		return m_name;
	}

private:
	std::FILE* m_outStream = stdout;
	std::FILE* m_errStream = stderr;
	std::string m_name;

	template <Level logLevel, class Format, typename ... Args> void log(Format&& fmt, Args&&... message) {
		if constexpr (static_cast<int>(config::disableLog) < static_cast<int>(logLevel)) {
			if constexpr (logLevel == Level::trace) {
				fmt::print(m_outStream, "{}[{:%Y-%m-%d %H:%M:%S}] [TRACE]: {}", ansi::set_style(ansi::Font::none, 81), fmt::localtime(std::chrono::system_clock::to_time_t(std::chrono::system_clock::now())), fmt::format(fmt::runtime(std::forward<Format>(fmt)), std::forward<Args>(message)...));
			} else if constexpr (logLevel == Level::debug) {
				fmt::print(m_outStream, "{}[{:%Y-%m-%d %H:%M:%S}] [DEBUG]: {}", ansi::set_style(ansi::Font::none, 242), fmt::localtime(std::chrono::system_clock::to_time_t(std::chrono::system_clock::now())), fmt::format(fmt::runtime(std::forward<Format>(fmt)), std::forward<Args>(message)...));
			} else if constexpr (logLevel == Level::info) {
				fmt::print(m_outStream, "{}[{:%Y-%m-%d %H:%M:%S}] [INFO]: {}", ansi::set_style(ansi::Font::none, 40), fmt::localtime(std::chrono::system_clock::to_time_t(std::chrono::system_clock::now())), fmt::format(fmt::runtime(std::forward<Format>(fmt)), std::forward<Args>(message)...));
			} else if constexpr (logLevel == Level::warning) {
				fmt::print(m_errStream, "{}[{:%Y-%m-%d %H:%M:%S}] [WARNING]: {}", ansi::set_style(ansi::Font::none, 184), fmt::localtime(std::chrono::system_clock::to_time_t(std::chrono::system_clock::now())), fmt::format(fmt::runtime(std::forward<Format>(fmt)), std::forward<Args>(message)...));
			} else if constexpr (logLevel == Level::error) {
				fmt::print(m_errStream, "{}[{:%Y-%m-%d %H:%M:%S}] [ERROR]: {}", ansi::set_style(ansi::Font::none, 197), fmt::localtime(std::chrono::system_clock::to_time_t(std::chrono::system_clock::now())), fmt::format(fmt::runtime(std::forward<Format>(fmt)), std::forward<Args>(message)...));
			} else if constexpr (logLevel == Level::exception) {
				fmt::print(m_errStream, "{}[{:%Y-%m-%d %H:%M:%S}] [EXCEPTION]: {}", ansi::set_style(ansi::Font::bold, 124), fmt::localtime(std::chrono::system_clock::to_time_t(std::chrono::system_clock::now())), fmt::format(fmt::runtime(std::forward<Format>(fmt)), std::forward<Args>(message)...));
			}
		} else return;
	}
	template <Level logLevel, class Msg> void log(Msg message) {
		if constexpr (static_cast<int>(config::disableLog) < static_cast<int>(logLevel)) {
			if constexpr (logLevel == Level::trace) {
				fmt::print(m_outStream, "{}[{:%Y-%m-%d %H:%M:%S}] [TRACE]: {}", ansi::set_style(ansi::Font::none, 81), fmt::localtime(std::chrono::system_clock::to_time_t(std::chrono::system_clock::now())), std::forward<Msg>(message));
			} else if constexpr (logLevel == Level::debug) {
				fmt::print(m_outStream, "{}[{:%Y-%m-%d %H:%M:%S}] [DEBUG]: {}", ansi::set_style(ansi::Font::none, 242), fmt::localtime(std::chrono::system_clock::to_time_t(std::chrono::system_clock::now())), std::forward<Msg>(message));
			} else if constexpr (logLevel == Level::info) {
				fmt::print(m_outStream, "{}[{:%Y-%m-%d %H:%M:%S}] [INFO]: {}", ansi::set_style(ansi::Font::none, 40), fmt::localtime(std::chrono::system_clock::to_time_t(std::chrono::system_clock::now())), std::forward<Msg>(message));
			} else if constexpr (logLevel == Level::warning) {
				fmt::print(m_errStream, "{}[{:%Y-%m-%d %H:%M:%S}] [WARNING]: {}", ansi::set_style(ansi::Font::none, 184), fmt::localtime(std::chrono::system_clock::to_time_t(std::chrono::system_clock::now())), std::forward<Msg>(message));
			} else if constexpr (logLevel == Level::error) {
				fmt::print(m_errStream, "{}[{:%Y-%m-%d %H:%M:%S}] [ERROR]: {}", ansi::set_style(ansi::Font::none, 197), fmt::localtime(std::chrono::system_clock::to_time_t(std::chrono::system_clock::now())), std::forward<Msg>(message));
			} else if constexpr (logLevel == Level::exception) {
				fmt::print(m_errStream, "{}[{:%Y-%m-%d %H:%M:%S}] [EXCEPTION]: {}", ansi::set_style(ansi::Font::bold, 124), fmt::localtime(std::chrono::system_clock::to_time_t(std::chrono::system_clock::now())), std::forward<Msg>(message));
			}
		} else return;
	}

	friend class LoggingContext;
};

Logger* const get(std::string_view name);

template <class Format, typename ... Args> inline void log(Format&& format, Args&&... message) {
	detail::default_logger()->log(std::forward<Format>(format), std::forward<Args>(message)...);
}
template <class Format, typename ... Args> inline void trace(Format&& format, Args&&... message) {
	detail::default_logger()->trace(std::forward<Format>(format), std::forward<Args>(message)...);
}
template <class Format, typename ... Args> inline void debug(Format&& format, Args&&... message) {
	detail::default_logger()->debug(std::forward<Format>(format), std::forward<Args>(message)...);
}
template <class Format, typename ... Args> inline void info(Format&& format, Args&&... message) {
	detail::default_logger()->info(std::forward<Format>(format), std::forward<Args>(message)...);
}
template <class Format, typename ... Args> inline void warning(Format&& format, Args&&... message) {
	detail::default_logger()->warning(std::forward<Format>(format), std::forward<Args>(message)...);
}
template <class Format, typename ... Args> inline void error(Format&& format, Args&&... message) {
	detail::default_logger()->error(std::forward<Format>(format), std::forward<Args>(message)...);
}
template <class Format, typename ... Args> inline void exception(Format&& format, Args&&... message) {
	detail::default_logger()->exception(std::forward<Format>(format), std::forward<Args>(message)...);	
}

template <class Msg> inline void log(Msg&& message) {
	detail::default_logger()->log(std::forward<Msg>(message));
}
template <class Msg> inline void trace(Msg&& message) {
	detail::default_logger()->trace(std::forward<Msg>(message));
}
template <class Msg> inline void debug(Msg&& message) {
	detail::default_logger()->debug(std::forward<Msg>(message));
}
template <class Msg> inline void info(Msg&& message) {
	detail::default_logger()->info(std::forward<Msg>(message));
}
template <class Msg> inline void warning(Msg&& message) {
	detail::default_logger()->warning(std::forward<Msg>(message));
}
template <class Msg> inline void error(Msg&& message) {
	detail::default_logger()->error(std::forward<Msg>(message));
}
template <class Msg> inline void exception(Msg&& message) {
	detail::default_logger()->exception(std::forward<Msg>(message));
}

inline void new_line() {
	detail::default_logger()->new_line();
}

} // namespace log

template <class Msg> constexpr void lyraAssert(bool condition, Msg message) {
	if (!condition) {
		log::exception(std::forward<Msg>(message));
		std::abort();
	}
}
template <class Msg> constexpr void VULKAN_ASSERT(VkResult function, Msg message) {
	if (function != VkResult::VK_SUCCESS) log::exception("Vulkan Exception: Failed to {} with error code: {}!", std::forward<Msg>(message), function);
}

template <class Format, typename ... Args> inline void lyraAssert(bool condition, Format&& format, Args&&... message) {
	if (!condition) {
		log::exception(format, std::forward<Args>(message)...);
		std::abort();
	}
}
template <class Format, typename ... Args> inline void VULKAN_ASSERT(VkResult function, Format&& format, Args&&... message) {
	if (function != VkResult::VK_SUCCESS) log::exception("Vulkan Exception: Failed to {} with error code: {}!", fmt::vformat(format, std::forward<Args>(message)...), function);
}

#define GET_ASSERT_OVERLOAD(_1, _2, _3, Name, ...) Name
#define ASSERT(...) GET_ASSERT_OVERLOAD(__VA_ARGS__, lyraAssert, lyraAssert)(__VA_ARGS__)
#define VULKAN_ASSERT(...) GET_ASSERT_OVERLOAD(__VA_ARGS__, VULKAN_ASSERT, VULKAN_ASSERT)(__VA_ARGS__)

} // namespace lyra
