#include <core/settings.h>

#include <res/loaders/load_file.h>

namespace lyra {

std::string Settings::init_json() const {
	std::string buffer;

	// load the file into the setring
	util::load_file("data/config.json", util::OpenMode::MODE_INPUT, buffer);

	return buffer;
}

const Settings& settings() {
	static Settings m_settings;
	return m_settings;
}

} // namespace lyra
