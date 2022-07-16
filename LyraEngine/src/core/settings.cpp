#include <core/settings.h>

namespace lyra {

void Settings::init() {
	// read the file and parse it into a json
	std::string file;
	load_file("data/config.json", OpenMode::MODE_START_AT_END | OpenMode::MODE_BINARY, &file);
	nlohmann::json json = nlohmann::json::parse(file);

	// set the variables
	// hooo booy, this is gonna be fuuun!
	Application::description = (char*)&json.at("application").at("description");
	Application::fps = (int)json.at("application").at("fps");

	Debug::debug = static_cast<DebugMode>((int)json.at("debug").at("debug"));
	Debug::disableLog = static_cast<DisableLog>((int)json.at("debug").at("disableLog"));
	Debug::printFPS = (bool)json.at("debug").at("printFPS");
	Debug::stdioSync = (bool)json.at("debug").at("stdioSync");

	Rendering::maxFramesInFlight = (uint8)json.at("rendering").at("maxFramesInFlight");
	Rendering::fov = (float)json.at("rendering").at("fov");
	Rendering::anistropy = (bool)json.at("rendering").at("anistropy");
	Rendering::anistropyStrength = (float)json.at("rendering").at("anistropyStrength");
	Rendering::resolution = (float)json.at("rendering").at("resolution");
	Rendering::polygonFrontFace = static_cast<PolygonFrontFace>((int)json.at("rendering").at("polygonFrontFace"));

	Window::title = (std::string)json.at("window").at("title");
	Window::iconPath = (std::string)json.at("window").at("iconPath");
	Window::width = (uint32)json.at("window").at("width");
	Window::height = (uint32)json.at("window").at("height");
	Window::wWidth = (uint32)json.at("window").at("wWidth");
	Window::wHeight = (uint32)json.at("window").at("wHeight");
	Window::resizable = (bool)json.at("window").at("resizable");
	Window::maximized = (bool)json.at("window").at("maximized");
	Window::borderless = (bool)json.at("window").at("borderless");
	Window::fullscreen = (bool)json.at("window").at("fullscreen");
	Window::alwaysOnTop = (bool)json.at("window").at("alwaysOnTop");
	Window::vSync = (bool)json.at("window").at("vSync"); // doesn't work yet

	Memory::maxComponentCount = (uint32)json.at("memory").at("maxComponentCount");
	Memory::maxEntityCount = (uint32)json.at("memory").at("maxEntityCount");
	Memory::maxCommandBuffers = (uint32)json.at("memory").at("maxCommandBuffers");
}

// i hate myself
char* Settings::Application::description;
int Settings::Application::fps;

Settings::DebugMode Settings::Debug::debug;
Settings::DisableLog Settings::Debug::disableLog;
bool Settings::Debug::printFPS;
bool Settings::Debug::stdioSync;
std::vector <const char*> Settings::Debug::requestedDeviceExtensions = { "VK_KHR_swapchain" };
std::vector <const char*> Settings::Debug::requestedValidationLayers = { "VK_LAYER_KHRONOS_validation" };

uint8 Settings::Rendering::maxFramesInFlight;
float Settings::Rendering::fov;
bool Settings::Rendering::anistropy;
float Settings::Rendering::anistropyStrength;
float Settings::Rendering::resolution;
Settings::PolygonFrontFace Settings::Rendering::polygonFrontFace;

std::string Settings::Window::title;
std::string Settings::Window::iconPath;
uint32 Settings::Window::width;
uint32 Settings::Window::height;
uint32 Settings::Window::wWidth;
uint32 Settings::Window::wHeight;
bool Settings::Window::resizable;
bool Settings::Window::maximized;
bool Settings::Window::borderless;
bool Settings::Window::fullscreen;
bool Settings::Window::alwaysOnTop;
bool Settings::Window::vSync;

uint32 Settings::Memory::maxComponentCount;
uint32 Settings::Memory::maxEntityCount;
uint32 Settings::Memory::maxCommandBuffers = 0;

} // namespace lyra
