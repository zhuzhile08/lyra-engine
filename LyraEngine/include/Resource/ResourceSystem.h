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

#include <Resource/LoadTextureFile.h>
#include <Resource/LoadMaterialFile.h>
#include <Resource/LoadMeshFile.h>

namespace lyra {

void initResourceSystem();

namespace resource {

const TextureFile& texture(const std::filesystem::path& name);
const MeshFile& mesh(const std::filesystem::path& name);
const MaterialFile& material(const std::filesystem::path& name);

} // namespace resource

} // namespace lyra