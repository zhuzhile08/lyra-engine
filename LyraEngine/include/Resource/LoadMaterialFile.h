/*************************
 * @file LoadMaterial.h
 * @author Zhile Zhu (zhuzhile08@gmail.com)
 * 
 * @brief Material loading functionality
 * 
 * @date 2023-04-22
 * 
 * @copyright Copyright (c) 2022
 *************************/

#pragma once

#include <Common/Common.h>


namespace lyra {

namespace resource {

struct MaterialFile {
	// std::vector<tinyobj::material_t> mats;
};

/**
 * @brief load matierals from a .mtl file
 *
 * @param path path of the model
 * 
 * @return lyra::resource::MaterialFile
 */
NODISCARD MaterialFile loadMaterial();

} // namespace resource

} // namespace lyra
