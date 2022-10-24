#include <core/settings.h>

#include <res/loaders/load_file.h>

namespace lyra {

std::string Settings::init_json() {
	std::string buffer;

	// load the file into the setring
	util::load_file("data/config.json", util::OpenMode::MODE_INPUT, buffer);

	return buffer;
}

const nlohmann::json Settings::json = nlohmann::json::parse(std::move(Settings::init_json()));

const char* Settings::Application::description = (char*)&json.at("application").at("description");
const int Settings::Application::fps = (int)json.at("application").at("fps");

const Settings::DebugMode Settings::Debug::debug = static_cast<DebugMode>((int)json.at("debug").at("debug"));
const Settings::DisableLog Settings::Debug::disableLog = static_cast<DisableLog>((int)json.at("debug").at("disableLog"));
const bool Settings::Debug::printFPS = (bool)json.at("debug").at("printFPS");
const bool Settings::Debug::stdioSync = (bool)json.at("debug").at("stdioSync");
const std::vector <const char*> Settings::Debug::requestedDeviceExtensions = { "VK_KHR_swapchain", "VK_KHR_portability_subset" };
const std::vector <const char*> Settings::Debug::requestedValidationLayers = { "VK_LAYER_KHRONOS_validation" };

const uint8 Settings::Rendering::maxFramesInFlight = (uint8)json.at("rendering").at("maxFramesInFlight");
const float Settings::Rendering::fov = (float)json.at("rendering").at("fov");
const bool Settings::Rendering::anistropy = (bool)json.at("rendering").at("anistropy");
const float Settings::Rendering::anistropyStrength = (float)json.at("rendering").at("anistropyStrength");
const float Settings::Rendering::resolution = (float)json.at("rendering").at("resolution");
const Settings::PolygonFrontFace Settings::Rendering::polygonFrontFace = static_cast<PolygonFrontFace>((int)json.at("rendering").at("polygonFrontFace"));

const std::string Settings::Window::title = (std::string)json.at("window").at("title");
const std::string Settings::Window::iconPath = (std::string)json.at("window").at("iconPath");
const uint32 Settings::Window::width = (uint32)json.at("window").at("width");
const uint32 Settings::Window::height = (uint32)json.at("window").at("height");
const uint32 Settings::Window::wWidth = (uint32)json.at("window").at("wWidth");
const uint32 Settings::Window::wHeight = (uint32)json.at("window").at("wHeight");
const bool Settings::Window::resizable = (bool)json.at("window").at("resizable");
const bool Settings::Window::maximized = (bool)json.at("window").at("maximized");
const bool Settings::Window::borderless = (bool)json.at("window").at("borderless");
const bool Settings::Window::fullscreen = (bool)json.at("window").at("fullscreen");
const bool Settings::Window::alwaysOnTop = (bool)json.at("window").at("alwaysOnTop");
const bool Settings::Window::vSync = (bool)json.at("window").at("vSync");

const uint32 Settings::Memory::maxComponentCount = (uint32)json.at("memory").at("maxComponentCount");
const uint32 Settings::Memory::maxEntityCount = (uint32)json.at("memory").at("maxEntityCount");
const uint32 Settings::Memory::maxCommandBuffers = (uint32)json.at("memory").at("maxCommandBuffers");

} // namespace lyra
