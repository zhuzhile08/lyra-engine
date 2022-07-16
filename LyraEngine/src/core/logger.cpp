#include <core/logger.h>

namespace lyra {

void Logger::init() {
#ifndef STDIO_SYNC
	std::ios::sync_with_stdio(true);
#endif
	_logFile.open("data/log/log.txt", std::ofstream::out | std::ofstream::trunc); // because I kinda can't use the logger functionality in here, you just have to hope that this doesn't throw an error
}

void Logger::clear_buffer() {
	set_color_default();
	std::cout << std::endl;
}

std::ofstream Logger::_logFile; // thanks, I hate this

} // namespace lyra