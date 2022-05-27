#include <graphics/renderer.h>

namespace lyra {

Renderer::Renderer() {
	Logger::log_info("Creating Renderer...");

	_framebuffers.create(Application::context()->device(), Application::context()->swapchain());

	VulkanDescriptorSetLayout::Builder layoutBuilder;
	layoutBuilder.add_binding({
		{0, VulkanDescriptor::Type::TYPE_UNIFORM_BUFFER, VulkanShader::Type::TYPE_VERTEX, 1},
		{1, VulkanDescriptor::Type::TYPE_IMAGE_SAMPLER, VulkanShader::Type::TYPE_FRAGMENT, 1}
	});

	VulkanDescriptorPool::Builder poolBuilder;
	poolBuilder.set_max_sets(4);
	poolBuilder.add_pool_sizes({
		{VulkanDescriptor::Type::TYPE_UNIFORM_BUFFER, Settings::Rendering::maxFramesInFlight},
		{VulkanDescriptor::Type::TYPE_IMAGE_SAMPLER, Settings::Rendering::maxFramesInFlight}
	});

	_pipeline.create(
		{
			&_framebuffers,
			{ { VulkanShader::Type::TYPE_VERTEX, "data/shader/vert.spv", "main" }, { VulkanShader::Type::TYPE_FRAGMENT, "data/shader/frag.spv", "main" } },
			layoutBuilder,
			poolBuilder,
			Application::context()->swapchain()->extent(),
			Application::context()->swapchain()->extent()
		}
	);

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

	_pipeline.bind();

	_draw_queue.flush();

	_framebuffers.end();
}

void Renderer::draw() noexcept {
	Application::context()->add_to_render_queue([&]() { record_command_buffers(); }); // this is what I call "spicy" architecture
}

void Renderer::bind_descriptor(const VulkanDescriptor* descriptor) const noexcept {
	vkCmdBindDescriptorSets(Application::context()->commandBuffers().at(Application::context()->currentFrame()).get(), VK_PIPELINE_BIND_POINT_GRAPHICS, _pipeline.layout(), 0, 1, descriptor->get_ptr(), 0, nullptr);
}

} // namespace lyra
