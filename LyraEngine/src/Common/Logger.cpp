#include <Common/Logger.h>

#ifdef _WIN32
#include <windows.h>
#endif

namespace lyra {

Logger::Logger() :
	m_logFile("data/log/log.txt", std::ofstream::out | std::ofstream::trunc)
{
	// create a virtual console if on windows
#ifndef STDIO_SYNC
	std::ios::sync_with_stdio(true);
#endif

#ifdef _WIN32
	DWORD outMode = 0;
	HANDLE stdoutHandle = GetStdHandle(STD_OUTPUT_HANDLE);

	GetConsoleMode(stdoutHandle, &outMode);
	outMode |= ENABLE_VIRTUAL_TERMINAL_PROCESSING;

	SetConsoleMode(stdoutHandle, outMode);
#endif
};

void Logger::clear_buffer() {
	set_color_default();
	std::cout << std::endl;
}

Logger& log() {
	static Logger* m_logger = new Logger();
	return *m_logger;
}

} // namespace lyra