#pragma once

#include <lyra.h>

#include <functional>

#include <core/smart_pointer.h>
#include <core/queue.h>

#include <rendering/renderer.h>

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
	SmartPointer<vulkan::DescriptorPool> m_descriptorPool;

	CallQueue m_drawQueue;

	/**
	 * @brief record the command buffer
	 */
	virtual void record_command_buffers() override;
};

} // namespace gui

} // namespace lyra