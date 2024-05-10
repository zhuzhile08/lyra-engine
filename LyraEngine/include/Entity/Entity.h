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

#include <Entity/Component.h>
#include <Entity/ECS.h>

#include <functional>
 
namespace lyra {

class Entity : public Node<Entity> {
public:
	constexpr Entity(std::string_view name = "Entity", EntityComponentSystem* ecs = ecs::globalECS) : 
		Node<Entity>(name), 
		m_id(ecs->addEntity(this)),
		m_ecs(ecs) { }
	constexpr Entity(const Entity&) = delete;
	constexpr Entity(Entity&&) = default;
	WIN32_CONSTEXPR ~Entity() {
		m_ecs->clearEntity(m_id);
		m_ecs->removeEntity(m_id);
	}

	Entity& operator=(const Entity&) = delete;
	Entity& operator=(Entity&&) = default;

	template <class Ty, class... Args> constexpr Entity& addComponent(Args&&... args) noexcept {
		m_ecs->addComponent<Ty>(m_id, std::forward<Args>(args)...);

		return *this;
	}
	template <class Ty> constexpr Entity& removeComponent() noexcept {
		m_ecs->removeComponent<Ty>();
		return *this;
	}
	template <class Ty> constexpr const Entity& removeComponent() const noexcept {
		m_ecs->removeComponent<Ty>();
		return *this;
	}
	WIN32_CONSTEXPR Entity& removeAll() noexcept {
		m_ecs->clearEntity(m_id);
		return *this;
	}
	WIN32_CONSTEXPR const Entity& removeAll() const noexcept {
		m_ecs->clearEntity(m_id);
		return *this;
	}

	template <class Ty> NODISCARD constexpr Ty& component() noexcept {
		return m_ecs->component<Ty>(m_id);
	}
	template <class Ty> NODISCARD constexpr const Ty& component() const {
		return m_ecs->component<Ty>(m_id);
	}

	template <class Ty> NODISCARD constexpr bool containsComponent() const {
		return m_ecs->containsComponent<Ty>(m_id);
		return false;
	}

	NODISCARD constexpr object_id id() const noexcept {
		return m_id;
	}

private:
	object_id m_id;
	EntityComponentSystem* m_ecs;
};

} // namespace lyra
