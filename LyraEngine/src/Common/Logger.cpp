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

	std::mutex defaultLoggerMutex;
	UniquePointer<Logger> defaultLogger;

	std::mutex loggerMutex;
	std::unordered_map<std::string, UniquePointer<Logger>> loggers;
};

}

static LoggingContext* globalLoggingContext = nullptr;

namespace log {

Logger* const logger(std::string_view name) {
	std::lock_guard<std::mutex> guard(globalLoggingContext->loggerMutex);
	return globalLoggingContext->loggers.find(name.data())->second.get();
}

Logger* const defaultLogger() {
	std::lock_guard<std::mutex> guard(globalLoggingContext->defaultLoggerMutex);
	return globalLoggingContext->defaultLogger.get();
}

} // namespace log

void initLoggingSystem() {
	if (globalLoggingContext) {
		log::error("initLoggingSystem(): The logging system is already initialized!");
		return;
	}

	globalLoggingContext = new LoggingContext();
}


Logger::Logger(FILE* out, FILE* err, std::string_view name)
 : m_outStream(out), m_errStream(err), m_name(name) { 
	globalLoggingContext->loggers.emplace(name, UniquePointer<Logger>::create(*this));
}
Logger::Logger(FILE* stream, std::string_view name) 
 : m_outStream(stream), m_errStream(stream), m_name(name) { 
	globalLoggingContext->loggers.emplace(name, UniquePointer<Logger>::create(*this));
}

} // namespace lyra

