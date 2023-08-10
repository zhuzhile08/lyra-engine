/*************************
 * @file Lyra.h
 * @author Zhile Zhu (zhuzhile08@gmail.com)
 * 
 * @brief The Engine header including all other headers as well as some initialization functions
 * 
 * @date 2023-06-24
 * 
 * @copyright Copyright (c) 2022
 *************************/

#pragma once

#include <Graphics/VulkanRenderSystem.h>
#include <Graphics/SDLWindow.h>

namespace lyra {

bool init(const Window& window) {
	return vulkan::init_render_system({
		{0, 7, 0},
		window.get()
	});
}

}