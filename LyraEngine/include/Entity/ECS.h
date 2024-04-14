/*************************
 * @file ECS.h
 * @author Zhile Zhu (zhuzhile08@gmail.com)
 * 
 * @brief Entity Component System API
 * 
 * @date 2024-01-31
 * 
 * @copyright Copyright (c) 2024
 *************************/

#pragma once

#include <Common/Common.h>
#include <Common/FunctionPointer.h>
#include <Common/Vector.h>
#include <Common/UnorderedSparseMap.h>
#include <Common/Benchmark.h>

#include <Entity/Component.h>

#include <typeindex>

namespace lyra {

void initECS();

class EntityComponentSystem {
private:
	class EntityManager {
	public:
		EntityManager() = default;

		constexpr objectid uniqueID() noexcept {
			if (m_unused.empty()) {
				return m_entities.size();
			} else {
				auto id = m_unused.back();
				m_unused.popBack();
				return id;
			}
		}
		constexpr void addEntity(Entity* e) noexcept {
			m_entities.pushBack(e);
		}

		constexpr Entity* entity(objectid id) noexcept {
			return m_entities[id];
		}
		constexpr const Entity* entity(objectid id) const noexcept {
			return m_entities[id];
		}
		constexpr void returnEntity(objectid id) noexcept {
			m_unused.pushBack(id);
			m_entities[id] = nullptr;
		}

	private:
		Vector<Entity*> m_entities;
		Vector<objectid> m_unused;

		friend class Entity;
	};

	class BasicComponentAllocator {
	public:
		constexpr BasicComponentAllocator() noexcept = default;
		constexpr BasicComponentAllocator(const BasicComponentAllocator&) noexcept = default;
		constexpr BasicComponentAllocator(BasicComponentAllocator&&) noexcept = default;
		constexpr virtual ~BasicComponentAllocator() noexcept = default;
		constexpr virtual void returnComponent(objectid id) noexcept = 0;
	};

	template <class Ty> class ComponentAllocator : public BasicComponentAllocator {
	public:
		using value_type = Ty;

		ComponentAllocator() = default;
		ComponentAllocator(const ComponentAllocator&) = default;
		ComponentAllocator(ComponentAllocator&&) = default;

		template <class... Args> constexpr value_type& allocateComponent(objectid& id, Args&&... args) noexcept {
			if (m_unused.empty()) {
				id = m_data.size();
				return m_data.emplaceBack(std::forward<Args>(args)...);
			} else {
				id = m_unused.back();
				m_unused.popBack();
				return *(new (&m_data[id]) value_type(std::forward<Args>(args)...));
			}
		}

		constexpr void returnComponent(objectid id) noexcept {
			m_data[id].~value_type();
			m_unused.pushBack(id);
		}

		constexpr value_type& component(objectid id) noexcept {
			return m_data[id];
		}
		constexpr const value_type& component(objectid id) const noexcept {
			return m_data[id];
		}

	private:
		Vector<value_type> m_data;
		Vector<objectid> m_unused;
	};

	using basic_allocator = UniquePointer<BasicComponentAllocator>;
	template <class Ty> using concrete_allocator = UniquePointer<ComponentAllocator<Ty>>;
	using allocators_type = UnorderedSparseMap<std::type_index, basic_allocator>;
	using entity_to_component = UnorderedSparseMap<objectid, objectid>;
	using lookup_type = UnorderedSparseMap<std::type_index, entity_to_component>;

public:
	// component functions
	template <class Ty, class... Args> constexpr Ty& addComponent(objectid e, Entity* p, Args&&... args) noexcept {
		std::type_index type(typeid(Ty));

		auto& component = dynamic_cast<ComponentAllocator<Ty>*>(
			m_componentAllocators.tryEmplace(type, concrete_allocator<Ty>::create()).first->second.get())->
				allocateComponent(m_lookup[type][e], std::forward<Args>(args)...);

		if constexpr (std::is_base_of_v<BasicComponent, Ty>) {
			component.entity = p;
			component.init();
		}

		return component;
	}
	template <class Ty> constexpr void removeComponent(objectid e) noexcept {
		std::type_index type(typeid(Ty));
		m_componentAllocators.at(type)->returnComponent(m_lookup[type].extract(e).second);
	} 
	constexpr void removeAllComponents(objectid e) noexcept {
		for (auto& mapping : m_lookup) {
			auto& comp = mapping.second;
			auto it = comp.find(e);

			if (it != comp.end()) {
				m_componentAllocators.at(mapping.first)->returnComponent(it->second);
				comp.erase(it);
			}
		}
	}


	template <class Ty> constexpr bool containsComponent(objectid e) const {
		return m_lookup.at(typeid(Ty)).contains(e);
	}

	template <class Ty> constexpr Ty& component(objectid e) noexcept {
		std::type_index type(typeid(Ty));
		return dynamic_cast<ComponentAllocator<Ty>*>(m_componentAllocators.at(type).get())->component(m_lookup[type][e]);
	}
	template <class Ty> constexpr const Ty& component(objectid e) const noexcept {
		std::type_index type(typeid(Ty));
		return dynamic_cast<ComponentAllocator<Ty>*>(m_componentAllocators.at(type).get())->component(m_lookup.at(type).at(e));
	}


	template <class... Types> constexpr Vector<Entity*> findEntities() noexcept {
		Vector<Entity*> r;

		entity_to_component* smallest = nullptr;
		Vector<entity_to_component*> mappings;

		for (std::type_index type : { std::type_index(typeid(Types))... }) {
			auto it = m_lookup.find(type);

			if (it != m_lookup.end()) {
				if (!smallest) smallest = &it->second;
				else if (it->second.size() < smallest->size()) {
					mappings.pushBack(smallest);
					smallest = &it->second;
				} else mappings.pushBack(&it->second);
			}
		}

		for (auto object = smallest->begin(); object != smallest->end(); object++) {
			bool hasComponents = true;
			auto id = object->first;

			for (auto mapping = mappings.rbegin(); mapping != mappings.rend(); mapping++) { // because the components with the smaller amount of objects are sorted backwards
				if (!(*mapping)->contains(id)) {
					hasComponents = false;
					break;
				}
			}

			if (hasComponents) r.pushBack(m_entityManager.entity(id));
		}

		return r;
	}
	template <class... Types> constexpr Vector<const Entity*> findEntities() const noexcept {
		Vector<const Entity*> r;

		const entity_to_component* smallest = nullptr;
		Vector<const entity_to_component*> mappings;

		for (std::type_index type : { std::type_index(typeid(Types))... }) {
			auto it = m_lookup.find(type);

			if (it != m_lookup.end()) {
				if (!smallest) smallest = &it->second;
				else if (it->second.size() < smallest->size()) {
					mappings.pushBack(smallest);
					smallest = &it->second;
				} else mappings.pushBack(&it->second);
			}
		}

		for (auto object = smallest->begin(); object != smallest->end(); object++) {
			bool hasComponents = true;
			auto id = object->first;

			for (auto mapping = mappings.rbegin(); mapping != mappings.rend(); mapping++) { // because the components with the smaller amount of objects are sorted backwards
				if (!(*mapping)->contains(id)) {
					hasComponents = false;
					break;
				}
			}

			if (hasComponents) r.pushBack(m_entityManager.entity(id));
		}

		return r;
	}

	template <class... Types, class Callable> constexpr void executeSystem(Callable&& callable) {
		entity_to_component* smallest = nullptr;
		Vector<entity_to_component*> mappings;

		for (std::type_index type : { std::type_index(typeid(Types))... }) {
			auto it = m_lookup.find(type);

			if (it != m_lookup.end()) {
				if (!smallest) smallest = &it->second;
				else if (it->second.size() < smallest->size()) {
					mappings.pushBack(smallest);
					smallest = &it->second;
				} else mappings.pushBack(&it->second);
			}
		}

		for (auto object = smallest->begin(); object != smallest->end(); object++) {
			bool hasComponents = true;
			auto id = object->first;

			for (auto mapping = mappings.rbegin(); mapping != mappings.rend(); mapping++) { // because the components with the smaller amount of objects are sorted backwards
				if (!(*mapping)->contains(id)) {
					hasComponents = false;
					break;
				}
			}

			if (hasComponents) callable(*m_entityManager.entity(id), component<Types>(id)...);
		}
	}

private:
	EntityManager m_entityManager;
	allocators_type m_componentAllocators;
	
	lookup_type m_lookup; // vertical: component mappings, horizontal: entity mappings

	friend class Entity;
};

namespace ecs {

extern EntityComponentSystem* globalECS;

} // namespace ecs

} // namespace lyra
