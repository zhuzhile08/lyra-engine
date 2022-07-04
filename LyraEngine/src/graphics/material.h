/*************************
 * @file material.h
 * @author Zhile Zhu (zhuzhile08@gmail.com)
 *
 * @brief a material system
 *
 * @date 2022-05-27
 *
 * @copyright Copyright (c) 2022
 *************************/

#pragma once

#include <graphics/material_manager.h>
#include <core/rendering/vulkan/descriptor.h>
#include <graphics/renderer.h>
#include <lyra.h>

namespace lyra {

class Material {
public:
	Material() { }

	/**
	 * @brief create the material
	 * 
	 * @param manager asset manager which contains the assets
	 * @param texID texture ID
	 * @param piplID pipeline ID
	*/
	void create(const MaterialManager* manager, const uint32 texID, const uint32 piplID, const VulkanDescriptor::Writer writer) {
		Logger::log_info("Creating material...");

		this->manager = manager;
		_textureID = texID;
		_pipelineID = piplID;

		_descriptor.create(manager->pipeline(piplID)->descriptorSetLayout(), manager->pipeline(piplID)->descriptorPool(), writer);

		Logger::log_info("Successfully created material with asset manager at address: ", get_address(manager), 
			", a texture ID of: ", _textureID, " and a pipeline ID of: ", _pipelineID, " with address:", get_address(this));
	}

	/**
	 * @brief bind the material
	 * 
	 * @param renderer renderer to bind to
	*/
	void bind(Renderer* const renderer) const {
		renderer->add_to_draw_queue([&]() {
			vkCmdBindPipeline(Application::context()->commandBuffers().at(Application::context()->currentFrame()).get(), manager->pipeline(_pipelineID)->bindPoint(), manager->pipeline(_pipelineID)->pipeline());
			vkCmdBindDescriptorSets(Application::context()->commandBuffers().at(Application::context()->currentFrame()).get(), manager->pipeline(_pipelineID)->bindPoint(),
				manager->pipeline(_pipelineID)->layout(), 0, 1, _descriptor.get_ptr(), 0, nullptr);
		});
	}

	/**
	 * @brief get the texture ID
	 * 
	 * @return const uint32 
	*/
	[[nodiscard]] const uint32 textureID() { return _textureID; }
	/**
	 * @brief get the pipeline ID
	 *
	 * @return const uint32
	*/
	[[nodiscard]] const uint32 pipelineID() { return _pipelineID; }
	/**
	 * @brief get the descriptor set
	 *
	 * @return const lyra::VulkanDescriptor* const
	*/
	[[nodiscard]] const VulkanDescriptor* const descriptor() { return &_descriptor; }

private:
	uint32 _textureID, _pipelineID;
	VulkanDescriptor _descriptor;

	const MaterialManager* manager;
};

} // namespace lyra