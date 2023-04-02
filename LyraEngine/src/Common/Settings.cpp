#include <Common/Settings.h>

#include <Resource/LoadFile.h>

namespace lyra {

Settings::Settings() :
	m_json(nlohmann::json::parse(init_json().c_str())),

	application{
		(char*)&m_json["application"]["description"],
		(int)m_json["application"]["fps"]
	},

	debug{
		static_cast<DebugMode>((int)m_json["debug"]["debug"]),
		static_cast<DisableLog>((int)m_json["debug"]["disableLog"]),
		(bool)m_json["debug"]["printFPS"],
		(bool)m_json["debug"]["stdioSync"],
		{
			"VK_KHR_swapchain"
#ifdef _WIN32
		},
#elif __APPLE__
			, "VK_KHR_portability_subset" },
#endif
		{  }
	},

	rendering{
		(float)m_json["rendering"]["fov"],
		(bool)m_json["rendering"]["anistropy"],
		(float)m_json["rendering"]["anistropyStrength"],
		(float)m_json["rendering"]["resolution"]
	},

	window{
		(std::string)m_json["window"]["title"],
		(std::string)m_json["window"]["iconPath"],
		(uint32)m_json["window"]["width"],
		(uint32)m_json["window"]["height"],
		(uint32)m_json["window"]["wWidth"],
		(uint32)m_json["window"]["wHeight"],
		(bool)m_json["window"]["resizable"],
		(bool)m_json["window"]["maximized"],
		(bool)m_json["window"]["borderless"],
		(bool)m_json["window"]["fullscreen"],
		(bool)m_json["window"]["alwaysOnTop"],
		(bool)m_json["window"]["vSync"]
	},

	memory{
		(uint32)m_json["memory"]["maxComponentCount"],
		(uint32)m_json["memory"]["maxEntityCount"]
	}
{ };

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
