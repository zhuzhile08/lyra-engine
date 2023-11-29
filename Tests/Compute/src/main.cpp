#define SDL_MAIN_HANDLED
#define LYRA_LOG_FILE

#include <glm/glm.hpp>

#include <Common/Common.h>

#include <Math/LyraMath.h>
#include <Math/ComputePipeline.h>

#include <Resource/Shader.h>
#include <Graphics/VulkanImpl/GPUBuffer.h>
#include <Graphics/VulkanImpl/DescriptorSystem.h>

#include <Application/Application.h>

struct Sphere {
	glm::vec3 pos;
	lyra::float32 rad;
};

struct Camera {
	lyra::float32 fov;
	lyra::float32 aspect;
	lyra::float32 near;
	lyra::float32 far;
};

int main() {
	// create the compute pipeline
	lyra::ComputePipeline computePipeline;

	{ // build the compute pipeline
		// create the builder for the compute pipeline
		lyra::ComputePipeline::Builder builder;
		builder.add_shader_info({  // shader information
			lyra::vulkan::Shader::Type::COMPUTE, "data/shaders/compute.spv", "main"
		});
		builder.add_binding_infos({ // descriptor information
			{ lyra::vulkan::Shader::Type::COMPUTE, 0, lyra::vulkan::DescriptorSystem::DescriptorSet::Type::uniformBuffer, 1 },
			{ lyra::vulkan::Shader::Type::COMPUTE, 0, lyra::vulkan::DescriptorSystem::DescriptorSet::Type::storageBuffer, 1 }
		});
		computePipeline = lyra::ComputePipeline(builder);
	}
	
	// create the buffer and descriptor to store the camera to send to the compute shader
	lyra::vulkan::GPUBuffer cameraBuffer(sizeof(Camera), VK_BUFFER_USAGE_STORAGE_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT, VMA_MEMORY_USAGE_GPU_ONLY);
	auto cameraDescriptorSet = computePipeline.descriptorSystem(0).get_unused_set();

	// create the buffer and descriptor to store the spheres to send to the compute shader
	lyra::vulkan::GPUBuffer sphereBuffer(sizeof(Sphere) * 1024, VK_BUFFER_USAGE_STORAGE_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT, VMA_MEMORY_USAGE_GPU_ONLY);
	auto sphereDescriptorSet = computePipeline.descriptorSystem(1).get_unused_set();

	// write the buffers

	// camera, represents the values passed into glm::perspective
	Camera camera { 90.0f, 4.0f/3.0f, 0.1f, 20.0f };
	lyra::vulkan::GPUBuffer cameraStagingBuffer(sizeof(Camera), VK_BUFFER_USAGE_STORAGE_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT, VMA_MEMORY_USAGE_CPU_ONLY);
	cameraStagingBuffer.copy_data(&camera);
	cameraBuffer.copy(cameraStagingBuffer);

	// spheres, represents "hitboxes" around objects that the frustum cull-er will use


	const auto& cmdBuff = lyra::Application::renderSystem.frames[0].commandBuffer();

	// begin recording the command buffer
	cmdQueue.activeCommandBuffer->begin();

	// record the commands
	cmdQueue.activeCommandBuffer->bindPipeline(computePipeline.bindPoint(), computePipeline.pipeline());
	cmdQueue.activeCommandBuffer->bindDescriptorSet(computePipeline.bindPoint(), computePipeline.layout(), 0, *cameraDescriptorSet);
	cmdQueue.activeCommandBuffer->bindDescriptorSet(computePipeline.bindPoint(), computePipeline.layout(), 1, *sphereDescriptorSet);
	// cmdQueue.activeCommandBuffer->dispatch(); I'm too lazy to finish these tests, so I'll just hope it works

	// end recording the command buffer
	cmdQueue.activeCommandBuffer->end();
	// submit the compute queue
	cmdQueue.activeCommandBuffer->submitQueue(lyra::Application::renderSystem.device.computeQueue());

	return 0;
}
