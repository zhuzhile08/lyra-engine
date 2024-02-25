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
#include <Common/FunctionPointer.h>

#include <typeindex>

namespace lyra {

void initECS();

namespace ecs {

Entity* entity(objectid id);

void* addComponent(objectid type, size_t size, objectid e);
void removeComponent(objectid type, objectid e);
void removeAllComponents(objectid e);

void* component(objectid type, objectid e);
const void* c_component(objectid type, objectid e);
bool containsComponent(objectid type, objectid e);

Vector<Entity*> findEntities(std::initializer_list<objectid>&& types);
void executeSystem(std::initializer_list<objectid>&& types, void (*system)(void*, Entity*), void* c);

objectid uniqueID();

template <class> inline objectid typeID() {
    static objectid id = uniqueID();
    return id;
}

} // namespace ecs

} // namespace lyra
