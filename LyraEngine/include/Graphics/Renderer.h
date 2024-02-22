/*************************
 * @file Renderer.h
 * @author Zhile Zhu (zhuzhile08@gmail.com)
 * 
 * @brief Declaration for a renderer and function for the render system front end
 * @brief Implementations are always in a <GRAPHICS_API>RenderSystem.cpp file
 * 
 * @date 2023-10-29
 * @copyright Copyright (c) 2023
 *************************/

#pragma once

#include <Common/Common.h>
#include <Common/Array.h>
#include <Common/UniquePointer.h>

#include <Graphics/VulkanRenderSystem.h>

#include <Entity/Entity.h>

#include <unordered_map>

namespace lyra {

void initRenderSystem(
	const Array<uint32, 3>& version, 
	const Window& window, 
	std::string_view defaultVertexShaderPath = "shader/vert.spv", 
	std::string_view defaultFragmentShaderPath = "shader/frag.spv"
);
void quitRenderSystem();

namespace renderer {

bool beginFrame();
void endFrame();

void draw();

uint32 drawWidth();
uint32 drawHeight();
uint32 currentFrameIndex();

float32 framesPerSecond();
float32 deltaTime();

vulkan::GraphicsPipeline& graphicsPipeline(
	const vulkan::GraphicsPipeline::Builder& pipelineBuilder = { },
	const vulkan::GraphicsProgram::Builder& programBuilder = { }
);

void setScene(Entity& sceneRoot);
Entity& scene();

} // namespace renderer

} // namespace lyra
