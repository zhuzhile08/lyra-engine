/*************************
 * @file System.h
 * @author Zhile Zhu (zhuzhile08@gmail.com)
 * 
 * @brief Systems implementation for the ECS
 * 
 * @date 2024-02-09
 * 
 * @copyright Copyright (c) 2024
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
	constexpr System() = default;
	template <class Callable> constexpr System(Callable&& callable, EntityComponentSystem* ecs = ecs::globalECS) : 
		//m_id(ecs->addSystem()),
		m_ecs(ecs) { }
	constexpr System(System&&) = default;
	constexpr System(const System&) {
		
	}
	constexpr ~System() {
		//m_ecs->removeSystem();
	}

	constexpr System& operator=(System&&) = default;
	constexpr System& operator=(const System&) {
		
	}
	template <class Callable> constexpr System& operator=(Callable&& callable) {
		
	}

	template <class Callable> void execute() {
		//ecs::globalECS->executeSystem<Types...>(callable);
	}
	template <class Callable> void execute() const {
		//ecs::globalECS->executeSystem<Types...>(callable);
	}

	Vector<Entity*> query() {
		//return ecs::globalECS->query<Types...>();
	}
	Vector<const Entity*> query() const {
		//return ecs::globalECS->query<Types...>();
	}

private:
	Function<void(Types...)> m_system;
	object_id m_id;

	EntityComponentSystem* m_ecs;
};

} // namespace lyra
