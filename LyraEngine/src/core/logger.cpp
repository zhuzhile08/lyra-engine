#include <core/logger.h>

namespace lyra {

void Logger::clear_buffer() {
	set_color_default();
	std::cout << std::endl;
}

Logger& log() {
	static Logger* m_logger = new Logger();
	return *m_logger;
}

} // namespace lyra