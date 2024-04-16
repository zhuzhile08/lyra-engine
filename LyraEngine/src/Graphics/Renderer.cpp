#include <Graphics/Renderer.h>

#include <Graphics/VulkanRenderSystem.h>
#include <Graphics/Material.h>

#include <Entity/Entity.h>
#include <Entity/Components/Camera.h>
#include <Entity/Components/MeshRenderer.h>

namespace lyra {

namespace renderer {

extern vulkan::RenderSystem* globalRenderSystem;

bool beginFrame() {
	// calculate deltatime
	auto now = std::chrono::high_resolution_clock::now();
	renderer::globalRenderSystem->deltaTime = std::chrono::duration<float32, std::chrono::seconds::period>(now - renderer::globalRenderSystem->startTime).count();
	renderer::globalRenderSystem->startTime = now;

	if (!renderer::globalRenderSystem->swapchain->aquire()) return false;
	renderer::globalRenderSystem->swapchain->begin();
	renderer::globalRenderSystem->commandQueue->activeCommandBuffer->begin();
	return true;
}

void endFrame() {
	renderer::globalRenderSystem->commandQueue->activeCommandBuffer->end();
	renderer::globalRenderSystem->commandQueue->submit(renderer::globalRenderSystem->swapchain->renderFinishedFences[renderer::globalRenderSystem->swapchain->currentFrame]);
	renderer::globalRenderSystem->swapchain->present();
}

void draw() {
	auto& renderTargets = renderer::globalRenderSystem->renderTargets;
	auto& cameras = renderer::globalRenderSystem->cameras;
	auto& materials = renderer::globalRenderSystem->materials;
	auto& meshRenderers = renderer::globalRenderSystem->meshRenderers;

	auto cmd = renderer::globalRenderSystem->commandQueue->activeCommandBuffer;

	for (uint32 i = 0; i < renderTargets.size(); i++) {
		renderTargets[i]->begin();

		for (uint32 j = 0; j < cameras.size(); j++) {
			auto camera = cameras[j];
			
			for (auto& [graphicsPipeline, material] : materials) {
				if (std::holds_alternative<VkViewport>(graphicsPipeline->dynamicViewport)) {
					graphicsPipeline->dynamicViewport = VkViewport {
						0.0f,
						0.0f,
						static_cast<float32>(drawWidth()),
						static_cast<float32>(drawHeight()),
						0.0f,
						1.0f
					};
				}
				
				if (std::holds_alternative<VkRect2D>(graphicsPipeline->dynamicScissor)) {
					graphicsPipeline->dynamicScissor = VkRect2D {
						{
							static_cast<int32>(camera->viewportPosition.x * drawWidth()),
							static_cast<int32>(camera->viewportPosition.y * drawHeight())
						},
						{
							static_cast<uint32>(camera->viewportSize.x * drawWidth()),
							static_cast<uint32>(camera->viewportSize.y * drawHeight())
						},
					};
				}
				
				graphicsPipeline->bind();
				
				for (auto& [material, meshes] : meshRenderers) {
					
					material->m_descriptorSets.bind(currentFrameIndex());
					
					for (uint32 i = 0; i < meshes.size(); i++) {
						auto mesh = meshes[i];
						
						auto data = camera->data(mesh->entity->component<Transform>().globalTransform());
						
						cmd->pushConstants(
						   material->m_graphicsPipeline->program->pipelineLayout,
						   VK_SHADER_STAGE_VERTEX_BIT,
						   0,
						   sizeof(Camera::TransformData),
						   &data
						);
						
						cmd->bindVertexBuffer(mesh->m_vertexBuffer.buffer, 0, 0);
						cmd->bindIndexBuffer(mesh->m_indexBuffer.buffer, 0, VK_INDEX_TYPE_UINT32);
						cmd->drawIndexed(static_cast<uint32>(mesh->m_mesh->indices().size()), 1, 0, 0, 0);
					}
				}
			}
		}

		renderTargets[i]->end();
	}
}

uint32 drawWidth() {
	return renderer::globalRenderSystem->swapchain->extent.width;
}

uint32 drawHeight() {
	return renderer::globalRenderSystem->swapchain->extent.height;
}

uint32 currentFrameIndex() {
	return renderer::globalRenderSystem->swapchain->currentFrame;
}

float32 framesPerSecond() {
	return 1000 / renderer::globalRenderSystem->deltaTime;
}

float32 deltaTime() {
	return renderer::globalRenderSystem->deltaTime;
}

void setScene(Entity& sceneRoot) {
	renderer::globalRenderSystem->sceneRoot = &sceneRoot;

	auto loopEntity = [](const Entity& entity, auto&& func) -> void {
		for (const auto& e : entity) {
			if (e->containsComponent<Camera>()) {
				renderer::globalRenderSystem->cameras.pushBack(&e->component<Camera>());
			} if (e->containsComponent<MeshRenderer>()) {
				auto& m = e->component<MeshRenderer>();
				renderer::globalRenderSystem->materials[m.m_material->m_graphicsPipeline].pushBack(m.m_material);
				renderer::globalRenderSystem->meshRenderers[m.m_material].pushBack(&m);
			}

			func(*e, func);
		}
	};

	loopEntity(sceneRoot, loopEntity);
}

Entity& scene() {
	ASSERT(renderer::globalRenderSystem->sceneRoot, "lyra::renderer::scene(): An active scene wasn't set yet!");
	return *renderer::globalRenderSystem->sceneRoot;
}

vulkan::GraphicsPipeline& graphicsPipeline(
	const vulkan::GraphicsPipeline::Builder& pipelineBuilder,
	const vulkan::GraphicsProgram::Builder& programBuilder
) {
	auto renderSystem = renderer::globalRenderSystem;

	auto pipelineHash = pipelineBuilder.hash();

	if (!renderSystem->graphicsPipelines.contains(pipelineHash)) {
		auto programHash = programBuilder.hash();

		if (!renderSystem->graphicsPrograms.contains(programHash)) {
			vulkan::GraphicsPipeline::Builder modifiedPipelineBuilder = pipelineBuilder;
			modifiedPipelineBuilder.setGraphicsProgram(
				*renderSystem->graphicsPrograms.emplace(
					programHash, 
					new vulkan::GraphicsProgram(programBuilder)
				).first->second
			);

			return *renderSystem->graphicsPipelines.emplace(pipelineHash, new vulkan::GraphicsPipeline(modifiedPipelineBuilder)).first->second;
		}

		return *renderSystem->graphicsPipelines.emplace(pipelineHash, new vulkan::GraphicsPipeline(pipelineBuilder)).first->second;
	}

	return *renderSystem->graphicsPipelines.at(pipelineHash);
}

} // namespace renderer

void initRenderSystem(
	const Array<uint32, 3>& version,
	std::string_view defaultVertexShaderPath, 
	std::string_view defaultFragmentShaderPath) {
	if (renderer::globalRenderSystem)
		log::error("lyra::initRenderSystem(): The render system is already initialized!");
	else {
		renderer::globalRenderSystem = new vulkan::RenderSystem(version, defaultVertexShaderPath, defaultFragmentShaderPath);
		renderer::globalRenderSystem->initRenderComponents();
	}
}

void quitRenderSystem() {
	if (renderer::globalRenderSystem) vkDeviceWaitIdle(renderer::globalRenderSystem->device);
}

} // namespace lyra
