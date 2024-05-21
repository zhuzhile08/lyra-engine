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

#include <ECS/Entity.h>
#include <ECS/World/World.h>

#include <set>
#include <variant>

namespace lyra {

class BasicSystem {
public:
	virtual ~BasicSystem() = default;

	virtual void run() = 0;
	virtual object_id id() const noexcept = 0;
};

template <class... Types> class System : public BasicSystem {
public:
	/*
	System(ecs::World* ecs = ecs::globalECSWorld) : 
		m_id(m_world->addSystem(this)),
		m_world(ecs) { }
	constexpr System(System&&) = default;
	~System() {
		m_world->removeSystem(m_id);
	}
	*/

	void run() override {
		
	}
	void operator()() {
		
	}

	template <class Callable> void each(Callable&& callable) {
		m_callable = std::forward<Callable>(callable);
	}

	NODISCARD object_id id() const noexcept override {
		return m_id;
	}

private:
	ecs::World* m_world;
	size_type m_id;

	Function<void(Types&...)> m_callable;
};

} // namespace lyra
