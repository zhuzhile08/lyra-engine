#pragma once

#include <core/logger.h>

#include <vector>
#include <tiny_obj_loader.h>
#include <glm.hpp>

namespace lyra {

namespace non_access {
/**
 * @brief a struct holding some "freshly" loaded model data
 * @brief used as a hidden middle step, since a .obj model can have multiple seperate models inside
 */
struct LoadedModel {
	tinyobj::attrib_t                   vertices;
	std::vector<tinyobj::shape_t>       shapes;
	std::vector<tinyobj::material_t>    materials;
};

} // namespace non_access

/**
 * @brief load a model from a .obj file
 *
 * @param path path of the model
 * 
 * @return const non_access::LoadedModel
 */
const non_access::LoadedModel load_model(const char* path);

} // namespace lyra
