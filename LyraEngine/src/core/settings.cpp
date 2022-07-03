#include <core/settings.h>

namespace lyra {

void Settings::init() {
	// load the file
	std::ifstream file;
	file.open("data/config.json", std::ifstream::ate | std::ifstream::binary);
	if (!file.is_open()) std::abort(); // I hate circular inclusion

	int length = 0;
	file.seekg(0, std::ios::end); // move the cursor to the end of the file to read the length
	length = file.tellg();
	file.seekg(0, std::ios::beg); // move the cursor back to the beginning

	// read the file and parse it into a json
	std::string contents;
	contents.resize(length);
	file.read(contents.data(), length);
	nlohmann::json json = nlohmann::json::parse(contents);

	// set the variables
	// hooo booy, this is gonna be fuuun!
	Application::description = (char*)&json["application"]["description"];
	Application::fps = (int)json["application"]["fps"];

	Debug::debug = static_cast<DebugMode>((int)json["debug"]["debug"]);
	Debug::disableLog = static_cast<DisableLog>((int)json["debug"]["disableLog"]);
	Debug::printFPS = (bool)json["debug"]["printFPS"];
	Debug::stdioSync = (bool)json["debug"]["stdioSync"];

	Rendering::maxFramesInFlight = (uint8)json["rendering"]["maxFramesInFlight"];
	Rendering::fov = (float)json["rendering"]["fov"];
	Rendering::polygonFrontFace = static_cast<PolygonFrontFace>((int)json["rendering"]["polygonFrontFace"]);

	Window::title = (std::string)json["window"]["title"];
	Window::iconPath = (std::string)json["window"]["iconPath"];
	Window::width = (uint32)json["window"]["width"];
	Window::height = (uint32)json["window"]["height"];
	Window::wWidth = (uint32)json["window"]["wWidth"];
	Window::wHeight = (uint32)json["window"]["wHeight"];
	Window::resizable = (bool)json["window"]["resizable"];
	Window::maximized = (bool)json["window"]["maximized"];
	Window::borderless = (bool)json["window"]["borderless"];
	Window::fullscreen = (bool)json["window"]["fullscreen"];
	Window::alwaysOnTop = (bool)json["window"]["alwaysOnTop"];
	Window::vSync = (bool)json["window"]["vSync"]; // doesn't work yet

	file.close();
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

} // namespace lyra
