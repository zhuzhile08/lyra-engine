#include <core/settings.h>

namespace lyra {

void Settings::init() {
	nlohmann::json json;

	// load the file
	std::ifstream file;
	file.open("data/config.json", std::ifstream::ate | std::ifstream::binary);
	if (!file.is_open()) std::abort(); // I hate circular inclusion

	// move the cursor to the beginning of the file
	file.seekg(0);
	// read the file and parse it into a json
	char* contents = { };
	file.read(contents, file.tellg());
	json.parse(contents);

	// set the variables
	// hooo booy, this is gonna be fuuun!
	Application::description = (char*)&json["application"]["description"];
	Application::fps = (int)json["application"]["fps"];

	Debug::debug = (DebugMode)json["debug"]["debug"];
	Debug::disableLog = (DisableLog)json["debug"]["disableLog"];
	Debug::printFPS = (bool)json["debug"]["printFPS"];
	Debug::stdioSync = (bool)json["debug"]["stdioSync"];

	uint32 extensionCount = (uint32)json["debug"]["extensionCount"];
	for (const auto& extension : json["debug"]["requestedDeviceExtensions"]) Debug::requestedDeviceExtensions.push_back((const char*)&extension);

	uint32 validationCount = (uint32)json["debug"]["validationCount"];
	for (const auto& validation : json["debug"]["requestedValidationLayers"]) Debug::requestedValidationLayers.push_back((const char*)&validation);

	Rendering::maxFramesInFlight = (uint8)json["rendering"]["maxFramesInFlight"];
	Rendering::fov = (float)json["rendering"]["fov"];
	Rendering::resolution = (uint32)json["window"]["resolution"];
	Rendering::polygonFrontFace = (PolygonFrontFace)json["rendering"]["polygonFrontFace"]; // counter clockwise, just remember

	Window::title = (char*)&json["window"]["title"];
	Window::iconPath = (char*)&json["window"]["iconPath"];
	Window::width = (uint32)json["window"]["width"];
	Window::height = (uint32)json["window"]["height"];
	Window::resizable = (bool)json["window"]["resizable"];
	Window::borderless = (bool)json["window"]["borderless"];
	Window::fullscreen = (bool)json["window"]["fullscreen"];
	Window::alwaysOnTop = (bool)json["window"]["alwaysOnTop"];
	Window::vSync = (bool)json["window"]["vSync"]; // doesn't work yet
}

} // namespace lyra
