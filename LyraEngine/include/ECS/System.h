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
	virtual size_type generateHash() const = 0;
	virtual object_id id() const noexcept = 0;
};

template <class... Types> class System : public BasicSystem {
public:
	System(ecs::World* world = ecs::globalWorld) : 
		m_world(world) {
		m_world->insertSystem(this);
	}
	constexpr System(System&&) = default;
	~System() {
		m_world->eraseSystem(m_id);
	}

	void run() override {
		auto archetype = m_world->systemArchetype(m_id);
		if (archetype) archetype->template each<Types...>(m_callable);
	}
	void operator()() {
		run();
	}

	template <class Callable> void each(Callable&& callable) {
		m_callable = std::forward<Callable>(callable);
	}

	NODISCARD size_type generateHash() const override {
		std::set<type_id> container({ typeId<std::remove_cv_t<Types>>()... });

		auto hash = container.size();

		for (const auto& component : container) {
			hash ^= reinterpret_cast<uintptr>(component) + 0x9e3779b9 + (hash << 6) + (hash >> 2);
		}

		return hash;
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
