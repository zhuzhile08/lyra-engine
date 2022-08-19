#include <core/logger.h>

namespace lyra {

void Logger::clear_buffer() {
	set_color_default();
	std::cout << std::endl;
}

std::ofstream Logger::_logFile("data/log/log.txt", std::ofstream::out | std::ofstream::trunc);

} // namespace lyra