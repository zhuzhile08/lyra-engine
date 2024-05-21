/*************************
 * @file Entity.h
 * @author Zhile Zhu (zhuzhile08@gmail.com)
 * 
 * @brief Entity class
 * 
 * @date 2022-24-7
 * 
 * @copyright Copyright (c) 2022
*************************/

#pragma once

#include <Common/Common.h>
#include <Common/Logger.h>
#include <Common/Node.h>

#include <ECS/World/World.h>
 
namespace lyra {

class Entity : public Node<Entity> {
public:
	Entity(std::string_view name = "Entity", ecs::World* world = ecs::globalECSWorld) : 
		Node<Entity>(name), 
		m_world(world) {
		m_world->insertEntity(this);
	}
	Entity(const Entity&) = delete;
	Entity(Entity&&) = default;
	~Entity() {
		m_world->eraseEntity(m_id);
	}

	Entity& operator=(const Entity&) = delete;
	Entity& operator=(Entity&&) = default;

	template <class Ty, class... Args> Entity& addComponent(Args&&... args) noexcept {
		m_world->addComponent<Ty>(m_id, std::forward<Args>(args)...);
		return *this;
	}
	template <class Ty> Entity& removeComponent() noexcept {
		m_world->removeComponent<Ty>();
		return *this;
	}
	template <class Ty> const Entity& removeComponent() const noexcept {
		m_world->removeComponent<Ty>();
		return *this;
	}
	Entity& removeAll() noexcept {
		m_world->clearEntity(m_id);
		return *this;
	}
	const Entity& removeAll() const noexcept {
		m_world->clearEntity(m_id);
		return *this;
	}

	template <class Ty> NODISCARD Ty& component() noexcept {
		return m_world->component<Ty>(m_id);
	}
	template <class Ty> NODISCARD const Ty& component() const {
		return m_world->component<Ty>(m_id);
	}

	template <class Ty> NODISCARD bool containsComponent() const {
		return m_world->containsComponent<Ty>(m_id);
		return false;
	}

	NODISCARD object_id id() const noexcept {
		return m_id;
	}

private:
	object_id m_id;
	ecs::World* m_world;

	friend class ecs::EntityManager;
};

} // namespace lyra
