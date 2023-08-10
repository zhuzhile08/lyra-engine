#include <Common/Logger.h>

#include <ios>
#include <unordered_map>

#ifdef _WIN32
#include <windows.h>
#endif

namespace lyra {

namespace log {

class LoggingContext {
public:
	LoggingContext() {
		std::ios::sync_with_stdio(true);

#ifdef _WIN32
		DWORD outMode = 0;
		HANDLE stdoutHandle = GetStdHandle(STD_OUTPUT_HANDLE);

		GetConsoleMode(stdoutHandle, &outMode);
		outMode |= ENABLE_VIRTUAL_TERMINAL_PROCESSING;

		SetConsoleMode(stdoutHandle, outMode);
#endif

		defaultLogger = UniquePointer<Logger>::create();
	}

	std::mutex defaultLoggerMutex;
	UniquePointer<Logger> defaultLogger;

	std::mutex loggerMutex;
	std::unordered_map<std::string, UniquePointer<Logger>> loggers;
};

static LoggingContext* globalLoggingContext = nullptr;

Logger* const get(std::string_view name) {
	if (globalLoggingContext == nullptr) {
		globalLoggingContext = new LoggingContext();
	}

	std::lock_guard<std::mutex> guard(globalLoggingContext->loggerMutex);
	return globalLoggingContext->loggers.find(name.data())->second.get();
}

namespace detail {

void add_logger(const Logger& logger) {
	if (globalLoggingContext == nullptr) {
		globalLoggingContext = new LoggingContext();
	}

	globalLoggingContext->loggers.emplace(logger.name(), UniquePointer<Logger>::create(logger));
}

Logger* const default_logger() {
	if (globalLoggingContext == nullptr) {
		globalLoggingContext = new LoggingContext();
	}

	std::lock_guard<std::mutex> guard(globalLoggingContext->defaultLoggerMutex);
	return globalLoggingContext->defaultLogger.get();
}


} // namespace detail

} // namespace log

} // namespace lyra