/*************************
 * @file asset_manager.h
 * @author Zhile Zhu (zhuzhile08@gmail.com)
 *
 * @brief an asset manager
 * @brief manages textures and graphic pipelines
 *
 * @date 2022-05-26
 *
 * @copyright Copyright (c) 2022
 *************************/

#pragma once

#include <graphics/texture.h>
#include <graphics/graphics_pipeline.h>
#include <core/logger.h>

#include <vector>

namespace lyra {

// manager for assets
class AssetManager {
public:
	AssetManager() { }

	/**
	* @brief destroy the asset manager and all its contents
	**/
	~AssetManager() {
		_textures.clear();
		_pipelines.clear();

		Logger::log_info("Successfully destroyed asset manager and its contents!");
	}
	/**
	 * @brief manually destroy the asset manager and all its contents
	*/
	void destroy() {
		this->~AssetManager();
	}

	/**
	 * @brief add textures to the asset manager
	 *
	 * @param textures textures to add
	*/
	void add_textures(const std::initializer_list<const Texture*> textures) {
		_textures.insert(_textures.end(), textures);

		Logger::log_info("Inserted ", textures.size(), " textures into the asset manager at: ", get_address(this));
	}
	/**
	 * @brief add pipelines to the asset manager
	 *
	 * @param pipelines pipelines to add
	*/
	void add_pipelines(const std::initializer_list<const GraphicsPipeline*> pipelines) {
		_pipelines.insert(_pipelines.end(), pipelines);

		Logger::log_info("Inserted ", pipelines.size(), " graphics pipelines into the asset manager at: ", get_address(this));
	}

	/**
	 * @brief get a texture by index
	 *
	 * @param index index value used by the image
	 *
	 * @return const Texture* const
	*/
	[[nodiscard]] const Texture* const texture(int index) const { return _textures.at(index); }
	/**
	 * @brief get a image by index
	 *
	 * @param index index value used by the pipeline
	 *
	 * @return const GraphicsPipeline* const
	*/
	[[nodiscard]] const GraphicsPipeline* const pipeline(int index) const { return _pipelines.at(index); }

private:
	std::vector<const Texture*> _textures;
	std::vector<const GraphicsPipeline*> _pipelines;
};

/************************
* Asset System usage guide
* 
* since specifying the index is a huge pain, I would recommend using a following scema:
* 
* lyra::AssetManager manager1;
* 
* enum Manager1TextureID : int {
*	ID_GRASS_TEXTURE,
*	ID_DIRT_TEXTURE,
*	ID_STONE_TEXTURE,
*	...
* }
* 
* enum Manager1PipelineID : int {
*	ID_CELL_SHADER,
*	ID_OUTLINE_SHADER,
*	ID_METAL_SHADER,
*	...
* }
* 
* lyra::Material {ID_STONE_TEXTURE, ID_METAL_SHADER};
* 
* not only does this make the accessing of the contained assets much easier, it also looks better and its easier than specify a meaningless integer value everytime you want to acceess a texture
************************/

} // namespace lyra