#include <Common/Logger.h>
#include <Common/UniquePointer.h>

#include <ios>
#include <mutex>
#include <unordered_map>

#ifdef _WIN32
#include <windows.h>
#endif

namespace lyra {

namespace {

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

	std::mutex mutex;

	UniquePointer<Logger> defaultLogger;
	std::unordered_map<std::string, UniquePointer<Logger>> loggers;
};

}

static LoggingContext* globalLoggingContext = nullptr;

namespace log {

Logger* const logger(std::string_view name) {
	std::lock_guard<std::mutex> guard(globalLoggingContext->mutex);
	return globalLoggingContext->loggers.find(name.data())->second.get();
}

UniquePointer<Logger> releaseLogger(std::string_view name) {
	return globalLoggingContext->loggers.extract(name.data()).mapped().release();
}

Logger* const defaultLogger() {
	std::lock_guard<std::mutex> guard(globalLoggingContext->mutex);
	return globalLoggingContext->defaultLogger.get();
}

Logger* const addLogger(UniquePointer<Logger>&& logger) {
	return globalLoggingContext->loggers.emplace(logger->name(), logger.release()).first->second.get();
}

UniquePointer<Logger> setDefaultLogger(UniquePointer<Logger>&& logger) {
	auto p = globalLoggingContext->defaultLogger.release();
	globalLoggingContext->defaultLogger = UniquePointer<Logger>(logger.release());
	return p;
}

} // namespace log

void initLoggingSystem() {
	if (globalLoggingContext) {
		log::error("initLoggingSystem(): The logging system is already initialized!");
		return;
	}

	globalLoggingContext = new LoggingContext();
}

} // namespace lyra

