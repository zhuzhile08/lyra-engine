/*************************
 * @file LoadMaterial.h
 * @author Zhile Zhu (zhuzhile08@gmail.com)
 * 
 * @brief Material loading functionality
 * 
 * @date 2023-04-22
 * 
 * @copyright Copyright (c) 2023
 *************************/

#pragma once

#include <Common/Common.h>


namespace lyra {

namespace resource {

struct MaterialFile {
	// lsd::Vector<tinyobj::material_t> mats;
};

NODISCARD MaterialFile loadMaterialFile(std::filesystem::path path);

} // namespace resource

} // namespace lyra
