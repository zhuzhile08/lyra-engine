/*************************
 * @file System.h
 * @author Zhile Zhu (zhuzhile08@gmail.com)
 * 
 * @brief Systems implementation for the ECS
 * 
 * @date 2024-02-09
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
		auto systemCallable = [&callable](Entity* entity) {
			callable(*entity, entity->component<Types>()...);
		};

		ecs::executeSystem(
			{ ecs::typeID<Types>()... }, 
			[](void* c, Entity* e) { (*reinterpret_cast<decltype(systemCallable)*>(c))(e); },
			&systemCallable
		);
	}

	Vector<Entity*> entites() const {
		return ecs::findEntities({ ecs::typeID<Types>()... });
	}
};

} // namespace lyra
