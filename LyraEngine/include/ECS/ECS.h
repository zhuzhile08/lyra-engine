/*************************
 * @file ECS.h
 * @author Zhile Zhu (zhuzhile08@gmail.com)
 * 
 * @brief Entity Component System API
 * 
 * @date 2024-01-31
 * 
 * @copyright Copyright (c) 2024
 *************************/

#pragma once

#include <Common/Common.h>

#include <ECS/Entity.h>
#include <ECS/Component.h>
#include <ECS/System.h>

#include <type_traits>

namespace lyra {

void initECS();

namespace ecs {

void update();

} // namespace ecs

} // namespace lyra
