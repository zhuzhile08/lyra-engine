/*************************
 * @file Entity.h
 * @author Zhile Zhu (zhuzhile08@gmail.com)
 * 
 * @brief Entity class
 * 
 * @date 2022-24-7
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
	Entity(std::string_view name = "Entity", Entity* parent = nullptr);
	Entity(std::string_view name, Entity& parent) : Entity(name, &parent) { }

	~Entity();

	Entity(Entity&&) = default;
	Entity& operator=(Entity&&) = default;

	template <class Ty, class... Args> Entity& addComponent(Args&&... args) {
		auto c = new (ecs::addComponent(ecs::typeID<Ty>(), sizeof(Ty), m_id)) Ty(std::forward<Args>(args)...);

		if constexpr (std::is_base_of_v<BasicComponent, Ty>) c->entity = this;

		return *this;
	}
	template <class Ty> Entity& removeComponent() {
		ecs::removeComponent(ecs::typeID<Ty>(), m_id);
		return *this;
	}
	template <class Ty> const Entity& removeComponent() const {
		ecs::removeComponent(ecs::typeID<Ty>(), m_id);
		return *this;
	}
	Entity& removeAll() {
		ecs::removeAllComponents(m_id);
		return *this;
	}
	const Entity& removeAll() const {
		ecs::removeAllComponents(m_id);
		return *this;
	}

	template <class Ty> NODISCARD Ty& component() {
		if (!ecs::containsComponent(ecs::typeID<Ty>(), m_id)) {
			addComponent<Ty>();
			log::error("lyra::Entity::component(): trying to access a component with internal type of: {} but it did't exist! Component was therefore added.", typeid(Ty).name());
		}

		return *reinterpret_cast<Ty*>(ecs::component(ecs::typeID<Ty>(), m_id));
	}
	template <class Ty> NODISCARD const Ty& component() const {
		if (!ecs::containsComponent(ecs::typeID<Ty>(), m_id)) { // bypass const
			auto c = new (ecs::addComponent(ecs::typeID<Ty>(), sizeof(Ty), m_id)) Ty();
			if constexpr (std::is_base_of_v<BasicComponent, Ty>) c->entity = this;
		}

		return *reinterpret_cast<Ty*>(ecs::c_component(typeID<Ty>(), m_id));
	}

	template <class Ty> NODISCARD bool containsComponent() const {
		return ecs::containsComponent(ecs::typeID<Ty>(), m_id);
	}

	NODISCARD constexpr objectid id() const noexcept {
		return m_id;
	}

private:
	objectid m_id;
};

} // namespace lyra
