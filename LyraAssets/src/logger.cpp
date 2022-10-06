#include "logger.h"

void Logger::clear_buffer() {
	set_color_default();
	std::cout << std::endl;
}

std::ofstream Logger::m_logFile("data/log/log.txt", std::ofstream::out | std::ofstream::trunc);
