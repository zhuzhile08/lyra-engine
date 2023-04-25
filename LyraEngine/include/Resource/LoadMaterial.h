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

#include <Lyra/Lyra.h>

#include <tiny_obj_loader.h>

namespace lyra {

namespace util {

/**
 * @brief a struct holding raw material data loaded from a .mtl file
 */
struct LoadedMaterial {
	std::vector<tinyobj::material_t> mats;
	std::string path;
};

/**
 * @brief load matierals from a .mtl file
 *
 * @param path path of the model
 * 
 * @return lyra::util::LoadedMaterial
 */
NODISCARD LoadedMaterial load_material(std::string_view path);

} // namespace util

} // namespace lyra
