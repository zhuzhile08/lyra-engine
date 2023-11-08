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

#include <unordered_map>

namespace lyra {

struct InitInfo {
	Array<uint32, 3> version;
	const Window* window;
};

void initRenderSystem(const InitInfo& info);
void quitRenderSystem();

namespace renderSystem {

class Renderer : public RenderObject {
public:
	Renderer() = default;
	Renderer(vulkan::Framebuffers& framebuffers, const vulkan::GraphicsProgram& graphicsProgram);
	~Renderer();

	void bindCamera(Camera& camera) {
		m_camera = &camera;
	}

	void draw();

private:
	UniquePointer<RendererImpl> m_impl;

	std::unordered_map<Material*, std::vector<MeshRenderer*>> m_meshes;

	Camera* m_camera;

	friend class lyra::MeshRenderer;
	friend class lyra::Camera;
	friend class lyra::Material;
};

bool beginFrame();
void endFrame();

void draw();

uint32 drawWidth();
uint32 drawHeight();
uint32 currentFrameIndex();

}

} // namespace lyra
