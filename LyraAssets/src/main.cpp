#define VERSION "1.00"

#include "ContentManager.h"
#include "SDLImGuiRenderer.h"
#include "GuiElements.h"
#include "IconsCodicons.h"

#include <Lyra/Lyra.h>

#include <Common/Logger.h>
#include <Common/FileSystem.h>

#include <Graphics/SDLWindow.h>

#include <Input/Input.h>

#include <imgui.h>
#include <imgui_internal.h>

int main(int argc, char* argv[]) {
	lyra::init();
	lyra::initFileSystem(argv);

	lyra::Window window("LyraAssets - Lyra Engine Assets Pipeline Tool", lyra::Window::Flags::resizable, {860, 645});
	SDLImGuiRenderer guiRenderer(window);
	
	lyra::initInputSystem(window, ImGui::GetCurrentContext());

	ContentManager contentManager;
	
	ImFontConfig config;
	config.MergeMode = true; 
	config.PixelSnapH = true; 
	config.GlyphMinAdvanceX = 15.0f;
	config.GlyphOffset = {0, 4.5f};
	ImWchar range[] =  { ICON_MIN_CI, ICON_MAX_16_CI, 0 };
	guiRenderer.setIconFont("data/fonts/codicon.ttf", config, range, 15.0f);

	gui::ButtonBar buttonBar(guiRenderer);
	gui::MainMenuBar mainMenuBar(window, guiRenderer);

	while (window.running()) {
		lyra::input::update();
		guiRenderer.draw();
	}

	lyra::quit();
}
