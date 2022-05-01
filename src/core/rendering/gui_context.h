#pragma once

#include <functional>

#include <imgui.h>
#include <backends/imgui_impl_vulkan.h>
#include <backends/imgui_impl_sdl.h>

#include <core/logger.h>
#include <core/queue_types.h>
#include <core/rendering/vulkan/descriptor.h>
#include <core/rendering/vulkan/command_buffer.h>
#include <core/rendering/vulkan/framebuffers.h>
#include <core/rendering/context.h>
#include <core/rendering/window.h>

namespace lyra {

namespace gui {

// Context and Renderer of the ImGui extension
class GUIContext {
public:
	GUIContext();

	/**
	* @brief destructor of the GUI context
	*/
	~GUIContext();

	/**
	 * @brief destroy an instance of the GUI context
	*/
	void destroy();

	/**
	 * @brief initialize an instance of the Vulkan and SDL version of the Dear ImGui libary
	 *
	 * @param context vulkan application context
	 * @param window window
	*/
	void create(lyra::Context* const context, const lyra::Window* const window);

	/**
	 * @brief add a draw call to the drawing queue
	 * 
	 * @param func function to add as a function pointer
	*/
	void add_draw_call(std::function<void()>&& func);

	/**
	 * @brief draw the current frame
	*/
	void draw();

	/**
	 * @brief get the descriptor pool local to the GUI context
	 * 
	 * @return const lyra::VulkanDescriptorPool* const
	*/
	[[nodiscard]] const VulkanDescriptorPool* const descriptorPool() const noexcept { return &_descriptorPool; };
	/**
	 * @brief get the renderer local to the GUI context
	 *
	 * @return const lyra::Framebuffers* const
	*/
	[[nodiscard]] const VulkanFramebuffers* const framebuffers() const noexcept { return &_framebuffers; };

private:
	VulkanDescriptorPool _descriptorPool;
	VulkanFramebuffers _framebuffers;
	CallQueue _drawQueue;

	const Context* context;
	const Window* window;
};

} // namespace gui

} // namespace lyra