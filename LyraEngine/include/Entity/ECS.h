/*************************
 * @file ECS.h
 * @author Zhile Zhu (zhuzhile08@gmail.com)
 * 
 * @brief Entity Component System API
 * 
 * @date 2024-01-31
 * @copyright Copyright (c) 2022
 *************************/

#pragma once

#include <Common/Common.h>

#include <typeindex>

namespace lyra {

void initECS();

namespace ecs {

void* addComponent(std::type_index type, size_t size, objectid e);
void removeComponent(std::type_index type, objectid e);
void removeAllComponents(objectid e);

void* component(std::type_index type, objectid e);
const void* c_component(std::type_index type, objectid e);
bool containsComponent(std::type_index type, objectid e);

Entity* entity(objectid id);
std::vector<Entity*> findEntities(std::initializer_list<std::type_index>&& types);

} // namespace ecs

} // namespace lyra
