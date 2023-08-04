#include <Common/Settings.h>

#include <Common/FileSystem.h>

namespace lyra {

Settings::Settings() { 
	ByteFile file("data/config.json");
	std::vector<char> fileData;
	file.read(fileData.data(), fileData.size());
	Json json = Json::parse(fileData);
	
	application = {
		json["application"]["description"].get<std::string>(),
		json["application"]["fps"].get<int>()
	};

	debug = {
		static_cast<DebugMode>(json["debug"]["debug"].get<int32>()),
		static_cast<DisableLog>(json["debug"]["disableLog"].get<int32>()),
		json["debug"]["printFPS"].get<bool>(),
		json["debug"]["stdioSync"].get<bool>(),
		{
			"VK_KHR_swapchain"
#ifdef _WIN32
		},
#elif __APPLE__
			, "VK_KHR_portability_subset" },
#endif
		{  }
	};

	rendering = {
		json["rendering"]["fov"].get<float32>(),
		json["rendering"]["anistropy"].get<bool>(),
		json["rendering"]["anistropyStrength"].get<float32>(),
		json["rendering"]["resolution"].get<float32>()
	};

	window = {
		json["window"]["title"].get<std::string>(),
		json["window"]["iconPath"].get<std::string>(),
		json["window"]["width"].get<uint32>(),
		json["window"]["height"].get<uint32>(),
		json["window"]["wWidth"].get<uint32>(),
		json["window"]["wHeight"].get<uint32>(),
		json["window"]["resizable"].get<bool>(),
		json["window"]["maximized"].get<bool>(),
		json["window"]["borderless"].get<bool>(),
		json["window"]["fullscreen"].get<bool>(),
		json["window"]["alwaysOnTop"].get<bool>(),
		json["window"]["vSync"].get<bool>()
	};

	memory = {
		json["memory"]["maxComponentCount"].get<uint32>(),
		json["memory"]["maxEntityCount"].get<uint32>()
	};
};

const Settings& settings() {
	static Settings m_settings;
	return m_settings;
}

} // namespace lyra
