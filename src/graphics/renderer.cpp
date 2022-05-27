#include <graphics/renderer.h>

namespace lyra {

Renderer::Renderer() {
	Logger::log_info("Creating Renderer...");

	_framebuffers.create(Application::context()->device(), Application::context()->swapchain());

	Logger::log_info("Successfully created Renderer at: ", get_address(this), "!");
}

Renderer::~Renderer() noexcept {
	Logger::log_info("Successfully destroyed a renderer!");
}

void Renderer::destroy() noexcept {
	this->~Renderer();
}

void Renderer::record_command_buffers() {
	// look at how D Y N A M I C this is
	_framebuffers.begin();

	_drawQueue.flush();

	_framebuffers.end();
}

void Renderer::draw() noexcept {
	Application::context()->add_to_render_queue([&]() { record_command_buffers(); });
}

} // namespace lyra
