#pragma once

#include <vector>
#include <tiny_obj_loader.h>

#include <core/util.h>

namespace lyra {

namespace util {
/**
 * @brief a struct holding some "freshly" loaded model data
 * @brief used as a hidden middle step, since a .obj model can have multiple seperate models inside
 */
struct LoadedModel {
	tinyobj::attrib_t				   vertices;
	std::vector<tinyobj::shape_t>	   shapes;
	std::vector<tinyobj::material_t>	materials;
};

/**
 * @brief load a model from a .obj file
 *
 * @param path path of the model
 * 
 * @return const lyra::util::LoadedModel
 */
NODISCARD const LoadedModel load_model(const char* path);

} // namespace util

} // namespace lyra
