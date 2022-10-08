#include <core/settings.h>

#include <res/loaders/load_file.h>

namespace lyra {

std::string Settings::init_json() {
	std::string buffer;

	// load the file into the setring
	util::load_file("data/config.json", util::OpenMode::MODE_INPUT, buffer);

	return buffer;
}

nlohmann::json Settings::json = nlohmann::json::parse(std::move(Settings::init_json()));

char* Settings::Application::description = (char*)&json.at("application").at("description");
int Settings::Application::fps = (int)json.at("application").at("fps");

Settings::DebugMode Settings::Debug::debug = static_cast<DebugMode>((int)json.at("debug").at("debug"));
Settings::DisableLog Settings::Debug::disableLog = static_cast<DisableLog>((int)json.at("debug").at("disableLog"));
bool Settings::Debug::printFPS = (bool)json.at("debug").at("printFPS");
bool Settings::Debug::stdioSync = (bool)json.at("debug").at("stdioSync");
std::vector <const char*> Settings::Debug::requestedDeviceExtensions = { "VK_KHR_swapchain", "VK_KHR_portability_subset" };
std::vector <const char*> Settings::Debug::requestedValidationLayers = { "VK_LAYER_KHRONOS_validation" };

uint8 Settings::Rendering::maxFramesInFlight = (uint8)json.at("rendering").at("maxFramesInFlight");
float Settings::Rendering::fov = (float)json.at("rendering").at("fov");
bool Settings::Rendering::anistropy = (bool)json.at("rendering").at("anistropy");
float Settings::Rendering::anistropyStrength = (float)json.at("rendering").at("anistropyStrength");
float Settings::Rendering::resolution = (float)json.at("rendering").at("resolution");
Settings::PolygonFrontFace Settings::Rendering::polygonFrontFace = static_cast<PolygonFrontFace>((int)json.at("rendering").at("polygonFrontFace"));

std::string Settings::Window::title = (std::string)json.at("window").at("title");
std::string Settings::Window::iconPath = (std::string)json.at("window").at("iconPath");
uint32 Settings::Window::width = (uint32)json.at("window").at("width");
uint32 Settings::Window::height = (uint32)json.at("window").at("height");
uint32 Settings::Window::wWidth = (uint32)json.at("window").at("wWidth");
uint32 Settings::Window::wHeight = (uint32)json.at("window").at("wHeight");
bool Settings::Window::resizable = (bool)json.at("window").at("resizable");
bool Settings::Window::maximized = (bool)json.at("window").at("maximized");
bool Settings::Window::borderless = (bool)json.at("window").at("borderless");
bool Settings::Window::fullscreen = (bool)json.at("window").at("fullscreen");
bool Settings::Window::alwaysOnTop = (bool)json.at("window").at("alwaysOnTop");
bool Settings::Window::vSync = (bool)json.at("window").at("vSync");

uint32 Settings::Memory::maxComponentCount = (uint32)json.at("memory").at("maxComponentCount");
uint32 Settings::Memory::maxEntityCount = (uint32)json.at("memory").at("maxEntityCount");
uint32 Settings::Memory::maxCommandBuffers = (uint32)json.at("memory").at("maxCommandBuffers");

} // namespace lyra
