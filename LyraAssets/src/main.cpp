#define VERSION "1.00"

#include "ContentManager.h"
#include "SDLImGuiRenderer.h"
#include "GuiElements.h"
#include "IconsCodicons.h"

#include <Lyra/Lyra.h>

#include <Common/Logger.h>
#include <Common/FileSystem.h>

#include <Graphics/SDLWindow.h>

#include <Input/InputSystem.h>

#include <imgui.h>
#include <imgui_internal.h>

int main(int argc, char* argv[]) {
	lyra::init();
	lyra::initLoggingSystem();
	lyra::initFileSystem(argv);

	lyra::log::setDefaultLogger(lyra::UniquePointer<lyra::Logger>::create(std::tmpfile(), "default"));

	lyra::Window window("LyraAssets - Lyra Engine Assets Pipeline Tool", lyra::Window::Flags::resizable, {860, 645});
	SDLImGuiRenderer guiRenderer(window);
	guiRenderer.enableDocking();

	ContentManager contentManager;

	ProgramState state;
	state.contentManager = &contentManager;
	state.running = &window.running();
	
	lyra::initInputSystem(window, ImGui::GetCurrentContext());
	
	ImFontConfig config;
	config.MergeMode = true; 
	config.PixelSnapH = true; 
	config.GlyphMinAdvanceX = 15.0f;
	config.GlyphOffset = {0, 4.5f};
	ImWchar range[] =  { ICON_MIN_CI, ICON_MAX_16_CI, 0 };
	guiRenderer.setIconFont("data/fonts/codicon.ttf", config, range, 15.0f);

	gui::MainMenuBar mainMenuBar(guiRenderer, state);
	gui::ButtonBar buttonBar(guiRenderer, state);
	gui::Window guiWindow(guiRenderer, state);

	while (window.running()) {
		lyra::input::update();
		guiRenderer.draw();

		if (!window.running()) {
			window.running() = contentManager.close();
		}
	}

	lyra::quit();

	return 0;
}
