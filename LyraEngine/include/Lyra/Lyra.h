#pragma once

#pragma once

#include <Graphics/VulkanRenderSystem.h>
#include <Graphics/SDLWindow.h>

namespace lyra {

bool init(const Window& window) {
    return vulkan::init_render_system({
        "Lyra Engine",
        {0, 7, 0},
        window.get(),
        { "VK_LAYER_KHRONOS_validation" },
        { "VK_KHR_swapchain", "VK_KHR_portability_subset", "VK_EXT_descriptor_indexing" }
    });
}

}