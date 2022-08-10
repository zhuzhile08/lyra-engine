#pragma once

#include <core/decl.h>
#include <core/queue_types.h>
#include <core/rendering/renderer.h>

#include <functional>
#include <memory>

#include <imgui.h>
#include <backends/imgui_impl_vulkan.h>
#include <backends/imgui_impl_sdl.h>

namespace lyra {

namespace gui {

// Renderer for the ImGui extension
class GUIRenderer : public Renderer {
public:
	/**
	 * @brief initialize an instance of the Vulkan and SDL version of the Dear ImGui libary
	*/
	GUIRenderer();

	/**
	* @brief destructor of the GUI context
	*/
	~GUIRenderer();

	/**
	 * @brief add a draw call to the drawing queue
	 * 
	 * @param func function to add as a function pointer
	*/
	void add_draw_call(std::function<void()>&& func);

private:
	std::unique_ptr<VulkanDescriptorPool> _descriptorPool;

	CallQueue _drawQueue;

	/**
	 * @brief record the command buffer
	 */
	void record_command_buffers() const override;
};

} // namespace gui

} // namespace lyra