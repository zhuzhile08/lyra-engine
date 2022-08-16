#include <core/logger.h>

namespace lyra {

void Logger::init() {
#ifndef STDIO_SYNC
	std::ios::sync_with_stdio(true);
#endif
}

void Logger::quit() {
	clear_buffer();
	_logFile.close();
}

void Logger::clear_buffer() {
	set_color_default();
	std::cout << std::endl;
}

std::ofstream Logger::_logFile("data/log/log.txt", std::ofstream::out | std::ofstream::trunc);

} // namespace lyra