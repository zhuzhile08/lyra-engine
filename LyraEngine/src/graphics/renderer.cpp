#include <graphics/renderer.h>

namespace lyra {

Renderer::Renderer() {
	Logger::log_info("Creating Renderer...");

	_framebuffers.create(Application::context()->device(), Application::context()->swapchain());
	const_cast<Context*>(Application::context())->add_to_recreate_queue(FUNC_PTR(
		_framebuffers.destroy();
		_framebuffers.create(Application::context()->device(), Application::context()->swapchain());
	));

	Logger::log_info("Successfully created Renderer at: ", get_address(this), "!");
}

Renderer::~Renderer() noexcept {
	Logger::log_info("Successfully destroyed a renderer!");
}

void Renderer::destroy() noexcept {
	this->~Renderer();
}

void Renderer::record_command_buffers() const {
	_framebuffers.begin();

	_drawQueue.flush();
	
	_framebuffers.end();
}

} // namespace lyra
