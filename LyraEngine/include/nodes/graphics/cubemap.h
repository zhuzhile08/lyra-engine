/*************************
 * @file   cubemap.h
 * @author Zhile Zhu (zhuzhile08@gmail.com)
 * 
 * @brief  a cubemap/skybox
 * 
 * @date   2022-19-8
 * 
 * @copyright Copyright (c) 2022
*************************/

#pragma once

#include <nodes/spatial.h>

#include <rendering/texture.h>
#include <nodes/mesh/mesh.h>
#include <nodes/mesh/mesh_renderer.h>

#include <array>

namespace lyra {

/**
class Cubemap : public Spatial {
public:
	 * @brief construct a new cubemap
	 * 
	 * @param path paths of the images for the cubemap

	Cubemap(std::array<const char*, 6> path) { }

private:
	std::array<Texture, 6> textures;
	// Mesh cube;
	// MeshRenderer cubeRenderer;
};
*/

} // namespace lyra
