/*************************
 * @file ResourceSystem.h
 * @author Zhile Zhu (zhuzhile08@gmail.com)
 *
 * @brief A system for managing engine resources
 *
 * @date 2022-06-19
 * @copyright Copyright (c) 2022
 *************************/

#pragma once

#include <Common/Common.h>

#include <Graphics/VulkanRenderSystem.h>
#include <Graphics/Texture.h>
#include <Graphics/Mesh.h>
#include <Graphics/Material.h>

namespace lyra {

void initResourceSystem();

namespace resource {

const vulkan::Shader& shader(std::filesystem::path name);
const Texture& texture(std::filesystem::path name);
const Material& material(std::filesystem::path name);
const Vector<Mesh>& mesh(std::filesystem::path name);

const Texture& defaultTexture();
const Texture& defaultNormal();
inline const Mesh& mesh(std::filesystem::path path, uint32 index) {
	return mesh(path)[index];
}

} // namespace resource

} // namespace lyra