#pragma once

#include <core/core.h>

#include <functional>

#include <imgui.h>
#include <backends/imgui_impl_vulkan.h>
#include <backends/imgui_impl_sdl.h>

namespace lyra {

namespace gui {

// Context and Renderer of the ImGui extension
class GUIContext {
public:
	/**
	 * @brief initialize an instance of the Vulkan and SDL version of the Dear ImGui libary
	*/
	GUIContext();

	/**
	* @brief destructor of the GUI context
	*/
	~GUIContext();

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
	void add_draw_call(std::function<void()>&& func);

	/**
	 * @brief get the descriptor pool local to the GUI context
	 * 
	 * @return const lyra::VulkanDescriptorPool* const
	*/
	[[nodiscard]] const VulkanDescriptorPool* const descriptorPool() const noexcept { return _descriptorPool; };
	/**
	 * @brief get the GUI renderer
	 *
	 * @return const lyra::Renderer* const
	*/
	[[nodiscard]] const Renderer* const renderer() const noexcept { return _renderer; };

private:
	VulkanDescriptorPool* _descriptorPool;
	Renderer* _renderer;

	CallQueue* _drawQueue;

	/**
	 * @brief bind the GUI
	*/
	void bind();
};

} // namespace gui

} // namespace lyra