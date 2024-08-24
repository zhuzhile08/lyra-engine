/*************************
 * @file Lyra.h
 * @author Zhile Zhu (zhuzhile08@gmail.com)
 * 
 * @brief The Engine header including all other headers as well as some initialization functions
 * 
 * @date 2023-06-24
 * 
 * @copyright Copyright (c) 2023
 *************************/

#pragma once

#ifdef _WIN32
#define SDL_MAIN_HANDLED
#endif

#include <Common/Common.h>
#include <Common/Logger.h>
#include <Common/FileSystem.h>

#include <LSD/Utility.h>

#include <Resource/ResourceSystem.h>

#include <ETCS/ECS.h>

#include <Graphics/Window.h>
#include <Graphics/Renderer.h>

#include <Input/InputSystem.h>

#include <glm/glm.hpp>

namespace lyra {

enum class InitFlags {
	loggingSystem = 0x00000001,
	fileSystem = 0x00000002 | loggingSystem,
	resourceSystem = 0x00000004 | fileSystem,
	ecs = 0x00000008 | loggingSystem,
	window = 0x00000010 | loggingSystem,
	extendedWindow = 0x00000020 | window,
	inputSystem = 0x00000040 | window,
	renderSystem = 0x00000080 | window | resourceSystem,

	all = loggingSystem | fileSystem | resourceSystem | ecs | window | inputSystem | renderSystem,
	allExtended = loggingSystem | fileSystem | resourceSystem | ecs | extendedWindow | inputSystem | renderSystem,
	none = 0x00000000
};

struct InitInfo {
	int argc = 0;
	char** argv = nullptr;

	lsd::Array<uint32, 3> version = {0, 0, 1};

	lsd::String windowName = "";
	Window::Flags windowFlags = Window::Flags::none;
	glm::ivec2 windowSize = { 640, 480 };
};

inline void init(InitFlags flags = InitFlags::all, InitInfo info = InitInfo()) {
	using namespace lsd::enum_operators;

	ASSERT(
		SDL_Init(SDL_INIT_TIMER | SDL_INIT_VIDEO | SDL_INIT_JOYSTICK | SDL_INIT_GAMEPAD | SDL_INIT_EVENTS) == 0, 
		"lyra::init(): SDL init error: {}!", SDL_GetError()
	);

	if ((flags & InitFlags::loggingSystem) == InitFlags::loggingSystem) initLoggingSystem();
	if ((flags & InitFlags::fileSystem) == InitFlags::fileSystem) {
		ASSERT(info.argv, "lyra::init(): File Ssstem init was requested, but no valid argv was provided!");
		initFileSystem(info.argv);
	}
	if ((flags & InitFlags::resourceSystem) == InitFlags::resourceSystem) initResourceSystem();
	if ((flags & InitFlags::ecs) == InitFlags::ecs) etcs::init();
	if ((flags & InitFlags::window) == InitFlags::window) {
		if ((flags & InitFlags::extendedWindow) == InitFlags::extendedWindow)
			initWindow(info.windowName, info.windowFlags, info.windowSize);
		else 
			initWindow();
	}
	if ((flags & InitFlags::inputSystem) == InitFlags::inputSystem) initInputSystem();
	if ((flags & InitFlags::renderSystem) == InitFlags::renderSystem) initRenderSystem(info.version);
}

inline void quit() {
	quitRenderSystem();
	SDL_Quit();
}

} // namespace lyra
