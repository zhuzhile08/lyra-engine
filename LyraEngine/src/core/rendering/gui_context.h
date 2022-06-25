#pragma once

#include <functional>

#include <imgui.h>
#include <backends/imgui_impl_vulkan.h>
#include <backends/imgui_impl_sdl.h>

#include <core/logger.h>
#include <core/queue_types.h>
#include <core/rendering/vulkan/descriptor.h>
#include <core/rendering/vulkan/command_buffer.h>
#include <graphics/renderer.h>
#include <lyra.h>

namespace lyra {

namespace gui {

// Context and Renderer of the ImGui extension
class GUIContext {
public:
	GUIContext() { }

	/**
	 * @brief initialize an instance of the Vulkan and SDL version of the Dear ImGui libary
	 *
	 * @param renderer renderer to render the GUI
	*/
	GUIContext(Renderer* const renderer);

	/**
	* @brief destructor of the GUI context
	*/
	~GUIContext() {
		ImGui_ImplVulkan_Shutdown();

		lyra::Logger::log_info("Successfully destroyed GUI context!");
	}

	/**
	 * @brief destroy an instance of the GUI context
	*/
	void destroy() {
		this->~GUIContext();
	}

	/**
	 * @brief add a draw call to the drawing queue
	 * 
	 * @param func function to add as a function pointer
	*/
	void add_draw_call(std::function<void()>&& func) {
		_drawQueue.add(std::move(func));
	}

	/**
	 * @brief bind the GUI
	*/
	void bind() const;

	/**
	 * @brief get the descriptor pool local to the GUI context
	 * 
	 * @return const lyra::VulkanDescriptorPool* const
	*/
	[[nodiscard]] const VulkanDescriptorPool* const descriptorPool() const noexcept { return &_descriptorPool; };

private:
	VulkanDescriptorPool _descriptorPool;
	CallQueue _drawQueue;

	Renderer* renderer;
};

} // namespace gui

} // namespace lyra