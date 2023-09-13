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

void initLoggingSystem();

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

#ifdef _WIN32
#define IS_FMT_CONSTEXPR
#else
#define IS_FMT_CONSTEXPR constexpr
#endif

inline IS_FMT_CONSTEXPR std::string setStyle(Font font, uint32 foreground, uint32 background) {
	if constexpr (config::coloredLog == true)
		return fmt::format("\033[{};38;5;{};48;5;{}m", static_cast<int>(font), foreground, background);
	else return "";
}
inline IS_FMT_CONSTEXPR std::string setStyle(Font font, uint32 foreground) {
	if constexpr (config::coloredLog == true)
		return fmt::format("\033[{};38;5;{}m", static_cast<int>(font), foreground);
	else return "";
}
inline constexpr std::string resetStyle() noexcept {
	if constexpr (config::coloredLog == true)
		return "\033[0;0;0m";	
	else return "";
}

} // namespace ansi

namespace log {

enum class Level {
	log,
	trace,
	debug,
	info,
	warning,
	error,
	exception
};

} // namespace log

class Logger {
public:
	Logger() = default;
	Logger(FILE* out, FILE* err, std::string_view name);
	Logger(FILE* stream, std::string_view name);

	template <class Format, typename ... Args> constexpr void log(Format&& format, Args&&... message) {
		fmt::print(m_outStream, fmt::runtime(std::forward<Format>(format)), std::forward<Args>(message)...);
	}
	template <class Format, typename ... Args> constexpr void trace(Format&& format, Args&&... message) {
		log<log::Level::trace>(std::forward<Format>(format), std::forward<Args>(message)...);
	}
	template <class Format, typename ... Args> constexpr void debug(Format&& format, Args&&... message) {
		log<log::Level::debug>(std::forward<Format>(format), std::forward<Args>(message)...);
	}
	template <class Format, typename ... Args> constexpr void info(Format&& format, Args&&... message) {
		log<log::Level::info>(std::forward<Format>(format), std::forward<Args>(message)...);
	}
	template <class Format, typename ... Args> constexpr void warning(Format&& format, Args&&... message) {
		log<log::Level::warning>(std::forward<Format>(format), std::forward<Args>(message)...);
	}
	template <class Format, typename ... Args> constexpr void error(Format&& format, Args&&... message) {
		log<log::Level::error>(std::forward<Format>(format), std::forward<Args>(message)...);
	}
	template <class Format, typename ... Args> constexpr void exception(Format&& format, Args&&... message) {
		log<log::Level::exception>(std::forward<Format>(format), std::forward<Args>(message)...);
	}

	template <class Msg> constexpr void log(Msg&& message) {
		fmt::print(m_outStream, std::forward<Msg>(message));
	}
	template <class Msg> constexpr void trace(Msg&& message) {
		log<log::Level::trace>(std::forward<Msg>(message));
	}
	template <class Msg> constexpr void debug(Msg&& message) {
		log<log::Level::debug>(std::forward<Msg>(message));
	}
	template <class Msg> constexpr void info(Msg&& message) {
		log<log::Level::info>(std::forward<Msg>(message));
	}
	template <class Msg> constexpr void warning(Msg&& message) {
		log<log::Level::warning>(std::forward<Msg>(message));
	}
	template <class Msg> constexpr void error(Msg&& message) {
		log<log::Level::error>(std::forward<Msg>(message));
	}
	template <class Msg> constexpr void exception(Msg&& message) {
		log<log::Level::exception>(std::forward<Msg>(message));
	}

	IS_FMT_CONSTEXPR void newLine() {
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

	template <log::Level logLevel, class Format, typename ... Args> void log(Format&& fmt, Args&&... message) {
		if constexpr (static_cast<int>(config::disableLog) < static_cast<int>(logLevel)) {
			if constexpr (logLevel == log::Level::trace) {
				fmt::print(m_outStream, "{}[{:%Y-%m-%d %H:%M:%S}] [TRACE]:\t{}\n", ansi::setStyle(ansi::Font::none, 81), fmt::localtime(std::chrono::system_clock::to_time_t(std::chrono::system_clock::now())), fmt::format(fmt::runtime(std::forward<Format>(fmt)), std::forward<Args>(message)...));
			} else if constexpr (logLevel == log::Level::debug) {
				fmt::print(m_outStream, "{}[{:%Y-%m-%d %H:%M:%S}] [DEBUG]:\t{}\n", ansi::setStyle(ansi::Font::none, 242), fmt::localtime(std::chrono::system_clock::to_time_t(std::chrono::system_clock::now())), fmt::format(fmt::runtime(std::forward<Format>(fmt)), std::forward<Args>(message)...));
			} else if constexpr (logLevel == log::Level::info) {
				fmt::print(m_outStream, "{}[{:%Y-%m-%d %H:%M:%S}] [INFO]:\t{}\n", ansi::setStyle(ansi::Font::none, 40), fmt::localtime(std::chrono::system_clock::to_time_t(std::chrono::system_clock::now())), fmt::format(fmt::runtime(std::forward<Format>(fmt)), std::forward<Args>(message)...));
			} else if constexpr (logLevel == log::Level::warning) {
				fmt::print(m_errStream, "{}[{:%Y-%m-%d %H:%M:%S}] [WARNING]:\t{}\n", ansi::setStyle(ansi::Font::none, 184), fmt::localtime(std::chrono::system_clock::to_time_t(std::chrono::system_clock::now())), fmt::format(fmt::runtime(std::forward<Format>(fmt)), std::forward<Args>(message)...));
			} else if constexpr (logLevel == log::Level::error) {
				fmt::print(m_errStream, "{}[{:%Y-%m-%d %H:%M:%S}] [ERROR]:\t{}\n", ansi::setStyle(ansi::Font::none, 197), fmt::localtime(std::chrono::system_clock::to_time_t(std::chrono::system_clock::now())), fmt::format(fmt::runtime(std::forward<Format>(fmt)), std::forward<Args>(message)...));
			} else if constexpr (logLevel == log::Level::exception) {
				fmt::print(m_errStream, "{}[{:%Y-%m-%d %H:%M:%S}] [EXCEPTION]:\t{}\n", ansi::setStyle(ansi::Font::bold, 124), fmt::localtime(std::chrono::system_clock::to_time_t(std::chrono::system_clock::now())), fmt::format(fmt::runtime(std::forward<Format>(fmt)), std::forward<Args>(message)...));
			}
		} else return;
	}
	template <log::Level logLevel, class Msg> void log(Msg message) {
		if constexpr (static_cast<int>(config::disableLog) < static_cast<int>(logLevel)) {
			if constexpr (logLevel == log::Level::trace) {
				fmt::print(m_outStream, "{}[{:%Y-%m-%d %H:%M:%S}] [TRACE]:\t{}\n", ansi::setStyle(ansi::Font::none, 81), fmt::localtime(std::chrono::system_clock::to_time_t(std::chrono::system_clock::now())), std::forward<Msg>(message));
			} else if constexpr (logLevel == log::Level::debug) {
				fmt::print(m_outStream, "{}[{:%Y-%m-%d %H:%M:%S}] [DEBUG]:\t{}\n", ansi::setStyle(ansi::Font::none, 242), fmt::localtime(std::chrono::system_clock::to_time_t(std::chrono::system_clock::now())), std::forward<Msg>(message));
			} else if constexpr (logLevel == log::Level::info) {
				fmt::print(m_outStream, "{}[{:%Y-%m-%d %H:%M:%S}] [INFO]:\t{}\n", ansi::setStyle(ansi::Font::none, 40), fmt::localtime(std::chrono::system_clock::to_time_t(std::chrono::system_clock::now())), std::forward<Msg>(message));
			} else if constexpr (logLevel == log::Level::warning) {
				fmt::print(m_errStream, "{}[{:%Y-%m-%d %H:%M:%S}] [WARNING]:\t{}\n", ansi::setStyle(ansi::Font::none, 184), fmt::localtime(std::chrono::system_clock::to_time_t(std::chrono::system_clock::now())), std::forward<Msg>(message));
			} else if constexpr (logLevel == log::Level::error) {
				fmt::print(m_errStream, "{}[{:%Y-%m-%d %H:%M:%S}] [ERROR]:\t{}\n", ansi::setStyle(ansi::Font::none, 197), fmt::localtime(std::chrono::system_clock::to_time_t(std::chrono::system_clock::now())), std::forward<Msg>(message));
			} else if constexpr (logLevel == log::Level::exception) {
				fmt::print(m_errStream, "{}[{:%Y-%m-%d %H:%M:%S}] [EXCEPTION]:\t{}\n", ansi::setStyle(ansi::Font::bold, 124), fmt::localtime(std::chrono::system_clock::to_time_t(std::chrono::system_clock::now())), std::forward<Msg>(message));
			}
		} else return;
	}
};

namespace log {

Logger* const logger(std::string_view name);
Logger* const defaultLogger();

template <class Format, typename ... Args> inline void log(Format&& format, Args&&... message) {
	defaultLogger()->log(std::forward<Format>(format), std::forward<Args>(message)...);
}
template <class Format, typename ... Args> inline void trace(Format&& format, Args&&... message) {
	defaultLogger()->trace(std::forward<Format>(format), std::forward<Args>(message)...);
}
template <class Format, typename ... Args> inline void debug(Format&& format, Args&&... message) {
	defaultLogger()->debug(std::forward<Format>(format), std::forward<Args>(message)...);
}
template <class Format, typename ... Args> inline void info(Format&& format, Args&&... message) {
	defaultLogger()->info(std::forward<Format>(format), std::forward<Args>(message)...);
}
template <class Format, typename ... Args> inline void warning(Format&& format, Args&&... message) {
	defaultLogger()->warning(std::forward<Format>(format), std::forward<Args>(message)...);
}
template <class Format, typename ... Args> inline void error(Format&& format, Args&&... message) {
	defaultLogger()->error(std::forward<Format>(format), std::forward<Args>(message)...);
}
template <class Format, typename ... Args> inline void exception(Format&& format, Args&&... message) {
	defaultLogger()->exception(std::forward<Format>(format), std::forward<Args>(message)...);	
}

template <class Msg> inline void log(Msg&& message) {
	defaultLogger()->log(std::forward<Msg>(message));
}
template <class Msg> inline void trace(Msg&& message) {
	defaultLogger()->trace(std::forward<Msg>(message));
}
template <class Msg> inline void debug(Msg&& message) {
	defaultLogger()->debug(std::forward<Msg>(message));
}
template <class Msg> inline void info(Msg&& message) {
	defaultLogger()->info(std::forward<Msg>(message));
}
template <class Msg> inline void warning(Msg&& message) {
	defaultLogger()->warning(std::forward<Msg>(message));
}
template <class Msg> inline void error(Msg&& message) {
	defaultLogger()->error(std::forward<Msg>(message));
}
template <class Msg> inline void exception(Msg&& message) {
	defaultLogger()->exception(std::forward<Msg>(message));
}

inline void newLine() {
	defaultLogger()->newLine();
}

} // namespace log

template <class Msg> inline constexpr void lyraAssert(bool condition, Msg message) {
	if (!condition) {
		log::exception(std::forward<Msg>(message));
		std::abort();
	}
}
template <class Msg> inline constexpr void vulkanAssert(VkResult function, Msg message) {
	if (function != VkResult::VK_SUCCESS) {
		log::exception("Vulkan Exception: Failed to {} with error code: {}!", std::forward<Msg>(message), function);
		std::abort();
	}
}

template <class Format, typename ... Args> inline constexpr void lyraAssert(bool condition, Format&& format, Args&&... message) {
	if (!condition) {
		log::exception(std::forward<Format>(format), std::forward<Args>(message)...);
		std::abort();
	}
}
template <class Format, typename ... Args> inline constexpr void vulkanAssert(VkResult function, Format&& format, Args&&... message) {
	if (function != VkResult::VK_SUCCESS) {
		log::exception("Vulkan Exception: Failed to {} with error code: {}!", fmt::format(fmt::runtime(std::forward<Format>(format)), std::forward<Args>(message)...), function);
		std::abort();
	}
}

} // namespace lyra
