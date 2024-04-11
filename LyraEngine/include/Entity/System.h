/*************************
 * @file System.h
 * @author Zhile Zhu (zhuzhile08@gmail.com)
 * 
 * @brief Systems implementation for the ECS
 * 
 * @date 2024-02-09
 * 
 * @copyright Copyright (c) 2022
 *************************/

#pragma once

#include <Common/Common.h>
#include <Common/FunctionPointer.h>

#include <Entity/Entity.h>
#include <Entity/ECS.h>

#include <typeindex>
#include <tuple>

namespace lyra {

template <class... Types> class System {
public:
	template <class Callable> void each(Callable&& callable) const {
		ecs::defaultECS()->executeSystem<Types...>(callable);
	}

	Vector<Entity*> entites() const {
		return ecs::defaultECS()->findEntities<Types...>();
	}
	Vector<const Entity*> centites() const {
		return ecs::defaultECS()->findEntities<Types...>();
	}
};

} // namespace lyra
