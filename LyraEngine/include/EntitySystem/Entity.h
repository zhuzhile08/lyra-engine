/*************************
 * @file Entity.h
 * @author Zhile Zhu (zhuzhile08@gmail.com)
 * 
 * @brief game object class
 * 
 * @date 2022-24-7
 * 
 * @copyright Copyright (c) 2022
*************************/

#pragma once

#include <Common/Common.h>
#include <Common/UniquePointer.h>
#include <Common/Node.h>

#include <EntitySystem/Script.h>

#include <type_traits>
#include <typeindex>
#include <unordered_map>

namespace lyra {

class ComponentBase { 
public:
	Entity* entity() { return m_entity; }
protected:
	Entity* m_entity;

	friend class Entity;
};

template <class Ty> concept DerivedComponentType = std::is_base_of_v<ComponentBase, Ty>;

class Entity : public Node<Entity> {
public:
	Entity(
		std::string_view name = "Entity",
		Entity* parent = nullptr,
		Script* script = nullptr, 
		uint32 tag = 0, 
		bool visible = true, 
		bool constant = false
	);
	Entity(
		Entity& parent, 
		std::string_view name = "Entity", 
		Script* script = nullptr, 
		uint32 tag = 0, 
		bool visible = true, 
		bool constant = false
	);
	Entity(Entity&& entityData);
	
	void update();

	template <DerivedComponentType Ty, class... Args> void addComponent(Args&&... args) {
		m_components.emplace(std::type_index(typeid(Ty)), UniquePointer<Ty>::create(std::forward<Args>(args)...)).first->second->m_entity = this;
	}
	template <DerivedComponentType Ty> void addComponent(UniquePointer<Ty>&& component) {
		m_components.emplace(std::type_index(typeid(Ty)), std::move(component)).first->second->m_entity = this;
	}

	template <DerivedComponentType Ty> NODISCARD Ty* component() {
		return static_cast<Ty*>(m_components[std::type_index(typeid(Ty))]);
	}
	template <DerivedComponentType Ty> NODISCARD Ty* component() const {
		return static_cast<Ty*>(m_components.at(std::type_index(typeid(Ty))));
	}

	template <DerivedComponentType Ty> NODISCARD bool containsComponent() {
		return m_components.contains(std::type_index(typeid(Ty)));
	}
	template <DerivedComponentType Ty> NODISCARD bool containsComponent() const {
		return m_components.contains(std::type_index(typeid(Ty)));
	}

	NODISCARD constexpr uint32 tag() const noexcept {
		return m_tag;
	}
	NODISCARD constexpr bool visible() const noexcept {
		return m_visible;
	}
	NODISCARD constexpr bool constant() const noexcept {
		return m_constant;
	}

private:
	uint32 m_tag;
	bool m_visible;
	bool m_constant;

	UniquePointer<Script> m_script;
	std::unordered_map<std::type_index, UniquePointer<ComponentBase>> m_components;
};

} // namespace lyra
