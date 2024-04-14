#define VERSION "1.00"

#define TINYGLTF_NO_EXTERNAL_IMAGE
#define TINYGLTF_NO_INCLUDE_STB_IMAGE
#define TINYGLTF_NO_INCLUDE_STB_IMAGE_WRITE
#define TINYGLTF_USE_CPP14

#define STB_IMAGE_IMPLEMENTATION
#define STB_IMAGE_WRITE_IMPLEMENTATION
#define TINYGLTF_IMPLEMENTATION

#include <stb_image.h>
#include <stb_image_write.h>
#include <tiny_gltf.h>

#include "ContentManager.h"
#include "SDLImGuiRenderer.h"
#include "GuiElements.h"
#include "IconsCodicons.h"

#include <Lyra/Lyra.h>

#include <Common/Logger.h>
#include <Common/FileSystem.h>

#include <Graphics/Window.h>
#include <Graphics/VulkanRenderSystem.h>

#include <Input/InputSystem.h>

#include <imgui.h>
#include <imgui_internal.h>

int main(int argc, char* argv[]) {
	lyra::init(lyra::InitFlags::allExtended, { 
		.argc = argc, 
		.argv = argv,
		.windowName = "LyraAssets - Lyra Engine Assets Pipeline Tool", 
		.windowFlags = lyra::Window::Flags::resizable | lyra::Window::Flags::vulkan, 
		.windowSize = {860, 645}
	});

	lyra::ByteFile logFile = lyra::tmpFile();
	lyra::log::setDefaultLogger(lyra::UniquePointer<lyra::Logger>::create(logFile.stream(), "default"));

	ContentManager contentManager;

	ProgramState state;
	state.contentManager = &contentManager;

	lyra::vulkan::ImGuiRenderer guiRenderer;
	guiRenderer.enableDocking();
	
	lyra::input::enableImGui(ImGui::GetCurrentContext());
	
	ImFontConfig config;
	config.MergeMode = true; 
	config.PixelSnapH = true;
	config.GlyphExtraSpacing.x = 15.0f;
	config.GlyphOffset = {0, 4.5f};
	ImWchar range[] = { ICON_MIN_CI, ICON_MAX_16_CI, 0 };
	guiRenderer.setIconFont("data/fonts/codicon.ttf", config, range, 15.0f);

	gui::MainMenuBar mainMenuBar(guiRenderer, state);
	gui::ButtonBar buttonBar(guiRenderer, state);
	gui::Window guiWindow(guiRenderer, state);

	while (!lyra::input::quit() && !state.quit) {
		lyra::input::update();
		if (lyra::input::quit()) {
			state.quit = contentManager.close();
			lyra::input::cancelQuit();
		}

		if (!lyra::renderer::beginFrame()) continue;
		//lyra::renderer::draw();

		guiRenderer.draw();

		lyra::renderer::endFrame();
	}

	lyra::quit();

	return 0;
}
