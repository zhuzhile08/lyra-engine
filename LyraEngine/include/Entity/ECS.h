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
#include <Common/SharedPointer.h>
#include <Common/Vector.h>
#include <Common/UnorderedSparseMap.h>
#include <Common/Benchmark.h>

#include <Entity/Component.h>

#include <typeindex>
#include <tuple>

namespace lyra {

void initECS();

class EntityComponentSystem {
private:
	class EntityManager {
	public:
		EntityManager() = default;

		constexpr object_id uniqueID() noexcept {
			if (m_unused.empty()) {
				return m_entities.size();
			} else {
				auto id = m_unused.back();
				m_unused.popBack();
				return id;
			}
		}
		constexpr void addEntity(object_id id, Entity* e) noexcept {
			m_entities.resize(id + 1, nullptr);
			m_entities[id] = e;
		}

		constexpr Entity* entity(object_id id) noexcept {
			return m_entities[id];
		}
		constexpr const Entity* entity(object_id id) const noexcept {
			return m_entities[id];
		}
		constexpr void returnEntity(object_id id) noexcept {
			m_unused.pushBack(id);
			m_entities[id] = nullptr;
		}

	private:
		Vector<Entity*> m_entities;
		Vector<object_id> m_unused;

		friend class Entity;
	};

	class ComponentAllocator {
	private:
		class BasicMemory {
		public:
			virtual constexpr ~BasicMemory() { }

			virtual constexpr void* emplaceBack(void*) = 0;
			virtual constexpr void removeComponent(size_type) = 0;

			virtual constexpr void* componentData(size_type) = 0;
			virtual constexpr const void* componentData(size_type) const = 0;

			virtual constexpr size_type size() const noexcept = 0;
			virtual constexpr size_type count() const noexcept = 0;

			virtual constexpr void* begin() noexcept = 0;
			virtual constexpr const void* begin() const noexcept = 0;
			virtual constexpr void* end() noexcept = 0;
			virtual constexpr const void* end() const noexcept = 0;

			virtual constexpr UniquePointer<BasicMemory> copyType() const = 0;
		};

		template <class Ty> class Memory : public BasicMemory {
			using value_type = Ty;

			constexpr void* emplaceBack(void* data) override {
				return &m_memory.emplaceBack(std::move(*static_cast<Ty*>(data)));
			}
			constexpr void removeComponent(size_type index) override {
				m_memory[index] = std::move(m_memory.back());
				m_memory.popBack();
			}

			constexpr void* componentData(size_type index) override {
				return &m_memory[index];
			}
			constexpr const void* componentData(size_type index) const override {
				return &m_memory[index];
			}

			constexpr size_type size() const noexcept override {
				return sizeof(value_type);
			}
			constexpr size_type count() const noexcept override {
				return m_memory.size();
			}

			constexpr void* begin() noexcept override {
				return m_memory.begin().get();
			}
			constexpr const void* begin() const noexcept override {
				return m_memory.cbegin().get();
			}
			constexpr void* end() noexcept override {
				return m_memory.end().get();
			}
			constexpr const void* end() const noexcept override {
				return m_memory.cend().get();
			}

			constexpr UniquePointer<BasicMemory> copyType() const override {
				return UniquePointer<Memory>::create();
			}
		
		private:
			Vector<value_type> m_memory;
		};

		constexpr ComponentAllocator() = default; // cursed

	public:
		constexpr ComponentAllocator(ComponentAllocator&&) = default;
		constexpr ComponentAllocator(const ComponentAllocator& other) : m_memory(other.m_memory->copyType()) { }

		template <class Ty> static constexpr ComponentAllocator create() {
			ComponentAllocator a;
			a.m_memory = UniquePointer<Memory<Ty>>::create();
			return a;
		}

		template <class Ty> constexpr void* emplaceBack(Ty&& component) {
			Ty cmp = std::move(component);
			return m_memory->emplaceBack(&cmp);
		}
		constexpr void* emplaceBackData(void* component) {
			return m_memory->emplaceBack(component);
		}
		constexpr void removeComponent(size_type index) {
			m_memory->removeComponent(index);
		}

		template <class Ty> constexpr Ty* component(size_type index) {
			return static_cast<Ty*>(m_memory->componentData(index));
		}
		template <class Ty> constexpr const Ty* component(size_type index) const {
			return static_cast<Ty*>(m_memory->componentData(index));
		}
		constexpr void* componentData(size_type index) {
			return m_memory->componentData(index);
		}
		constexpr const void* componentData(size_type index) const {
			return m_memory->componentData(index);
		}

		constexpr size_type size() const noexcept {
			return m_memory->size();
		}
		constexpr size_type count() const noexcept {
			return m_memory->count();
		}

		template <class Ty> constexpr Iterator<Ty> begin() noexcept {
			return Iterator<Ty>(static_cast<Ty*>(m_memory->begin()));
		}
		template <class Ty> constexpr Iterator<Ty> begin() const noexcept {
			return Iterator<Ty>(static_cast<Ty*>(m_memory->begin()));
		}
		template <class Ty> constexpr Iterator<Ty> end() noexcept {
			return Iterator<Ty>(static_cast<Ty*>(m_memory->end()));
		}
		template <class Ty> constexpr Iterator<Ty> end() const noexcept {
			return Iterator<Ty>(static_cast<Ty*>(m_memory->end()));
		}

	private:

		UniquePointer<BasicMemory> m_memory;
	};

	class Archetype {
	private:
		struct Edge { 
		public:
			Archetype* superset { };
			Archetype* subset { };
		};

		class Hasher {
		public:
			constexpr size_type operator()(const UniquePointer<Archetype>& archetype) const noexcept {
				return archetype->m_hash;
			}
			constexpr size_type operator()(size_type hash) const noexcept {
				return hash;
			}
		};

		class Equal {
		public:
			constexpr bool operator()(const UniquePointer<Archetype>& first, const UniquePointer<Archetype>& second) const noexcept {
				return first->m_hash == second->m_hash;
			}
			constexpr bool operator()(const UniquePointer<Archetype>& first, size_type second) const noexcept {
				return first->m_hash == second;
			}
			constexpr bool operator()(size_type first, const UniquePointer<Archetype>& second) const noexcept {
				return first == second->m_hash;
			}
			constexpr bool operator()(size_type first, size_type second) const noexcept {
				return first == second;
			}
		};

		using component_alloc = ComponentAllocator;
		using components = UnorderedSparseMap<type_id, component_alloc>;

		using edges = UnorderedSparseMap<type_id, Edge>;
		using entities = UnorderedSparseSet<object_id>;
		
	public:
		constexpr Archetype() = default;
		constexpr Archetype(Archetype&&) = default;

		template <class Ty> static constexpr size_type superHash(const Archetype& archetype) {
			auto compTypeId = typeId<Ty>();
			auto inserted = false;

			auto hash = archetype.m_components.size() + 1;

			for (const auto& component : archetype.m_components) {
				if (component.first > compTypeId) {
					inserted = true;
					hash ^= reinterpret_cast<uintptr>(compTypeId) + 0x9e3779b9 + (hash << 6) + (hash >> 2);
				}

				hash ^= reinterpret_cast<uintptr>(component.first) + 0x9e3779b9 + (hash << 6) + (hash >> 2);
			}
			if (!inserted) hash ^= reinterpret_cast<uintptr>(compTypeId) + 0x9e3779b9 + (hash << 6) + (hash >> 2);

			return hash;
		}
		template <class Ty> static constexpr size_type subHash(const Archetype& archetype) {
			auto compTypeId = typeId<Ty>();

			auto hash = archetype.m_components.size() - 1;

			for (const auto& component : archetype.m_components)
				if (component.first != compTypeId)
					hash ^= reinterpret_cast<uintptr>(component.first) + 0x9e3779b9 + (hash << 6) + (hash >> 2);

			return hash;
		}

		template <class Ty> static Archetype createSuper(Archetype& archetype, size_type hash) {
			Archetype a;
			a.m_hash = hash;

			{ // insert component in the proper ordered position
				auto compTypeId = typeId<Ty>();
				auto inserted = false;
				for (const auto& component : archetype.m_components) {
					if (component.first > compTypeId) {
						inserted = true;
						a.m_components.emplace(compTypeId, ComponentAllocator::create<Ty>());
					}

					a.m_components.emplace(component.first, component.second);
				}

				if (!inserted) a.m_components.emplace(compTypeId, ComponentAllocator::create<Ty>());
			}

			a.edge<Ty>().subset = &archetype;

			return a;
		}
		template <class Ty> static Archetype createSub(Archetype& archetype, size_type hash) {
			ASSERT(!archetype.m_components.empty(), "lyra::EntityComponentSystem::Archetype::createSub(): Cannot create subset archetype of empty archetype!");

			Archetype a;
			a.m_hash = hash;

			{ // insert component in the proper ordered position
				auto compTypeId = typeId<Ty>();
				for (const auto& component : archetype.m_components)
					if (component.first != compTypeId) // insert component if it is not of type Ty
						a.m_components.emplace(component.first, component.second);
			}

			a.edge<Ty>().superset = &archetype;

			return a;
		}

		template <class Ty> constexpr Ty& addSubEntity(object_id entityId, Archetype& subset, Ty&& component) {
			m_entities.emplace(entityId);

			auto& c = *static_cast<Ty*>(m_components.at(typeId<Ty>()).emplaceBack(std::move(component)));

			auto entityIndex = (subset.m_entities.find(entityId) - subset.m_entities.begin());

			for (auto& component : subset.m_components) {
				m_components.at(component.first).emplaceBackData(
					component.second.componentData(entityIndex)
				);
			}

			subset.removeEntity(entityId);

			return c;
		}
		template <class Ty> constexpr void addSuperEntity(object_id entityId, Archetype& superset) {
			m_entities.emplace(entityId);

			auto entityIndex = (superset.m_entities.find(entityId) - superset.m_entities.begin());

			auto id = typeId<Ty>();

			for (auto& component : superset.m_components)
				if (component.first != id) 
					m_components.at(component.first).emplaceBackData(component.second.componentData(entityIndex));

			superset.removeEntity(entityId);
		}
		void removeEntity(object_id entityId) {
			auto it = m_entities.find(entityId);

			if (it != m_entities.end()) {
				auto index = (it - m_entities.begin());
				m_entities.erase(it);

				for (auto& component : m_components) component.second.removeComponent(index);

				return;
			}

			log::error("lyra::EnitityComponentSystem::Archetype::removeEntity(): Archetype did not contain entity with ID: {} !", entityId);
		}

		template <class Ty> constexpr Ty& component(object_id entityId) {
			return *m_components.at(typeId<Ty>()).template component<Ty>(m_entities.at(entityId));
		}
		template <class Ty> constexpr const Ty& component(object_id entityId) const {
			return *m_components.at(typeId<Ty>()).template component<Ty>(m_entities.at(entityId));
		}

		template <class Ty> constexpr bool contains() const {
			return m_components.contains(typeId<Ty>());
		}

		template <class Ty> constexpr const Edge& edge() const {
			return m_edges.at(typeId<Ty>());
		}
		template <class Ty> constexpr Edge& edge() {
			return m_edges[typeId<Ty>()];
		}

		template <class... Types, class Callable> constexpr void each(const Callable& c) {

		}
		template <class... Types, class Callable> constexpr void each(const Callable& c) const {
			
		}

	private:
		components m_components;
		entities m_entities;

		edges m_edges;
		size_type m_hash = 0;

		friend class EntityComponentSystem;
		friend class Hasher;
		friend class Equal;
	};

	using archetype = UniquePointer<Archetype>;

	using entity_to_archetype = UnorderedSparseMap<object_id, Archetype*>;
	using archetypes = UnorderedSparseSet<archetype, Archetype::Hasher, Archetype::Equal>;

public:
	constexpr EntityComponentSystem() { m_archetypes.emplace(archetype::create()); }

	// entity functions

	constexpr object_id addEntity(Entity* entity) {
		auto id = m_entities.uniqueID();

		m_entities.addEntity(id, entity);

		auto* archetype = m_archetypes.at(0).get();
		archetype->m_entities.emplace(id);

		m_lookup.emplace(id, archetype);

		return id;
	}
	constexpr void removeEntity(object_id e) {
		m_entities.returnEntity(e);
		m_lookup.erase(e);
	}

	WIN32_CONSTEXPR void clearEntity(object_id e) {
		auto& archetype = m_lookup.at(e);

		archetype->removeEntity(e);
		archetype = m_archetypes.begin()->get();
	}


	// component functions

	template <class Ty, class... Args> constexpr Ty& addComponent(object_id entityId, Args&&... args) noexcept {
		auto archetypeIt = m_lookup.find(entityId);

		Archetype* base = (archetypeIt != m_lookup.end()) ? archetypeIt->second : m_archetypes.begin()->get();
		Archetype* archetype = addOrFindSuperset<Ty>(base);

		m_lookup.insertOrAssign(entityId, archetype);

		return archetype->addSubEntity(entityId, *base, Ty(std::forward<Args>(args)...));
	}
	template <class Ty> constexpr void removeComponent(object_id entityId) noexcept {
		auto archetypeIt = m_lookup.find(entityId);

		if (archetypeIt != m_lookup.end()) {
			Archetype* archetype = addOrFindSubset<Ty>(archetypeIt->second);
			archetype->addSuperEntity<Ty>(entityId, archetypeIt->second);

			m_lookup.insertOrAssign(entityId, archetype);
		}

		log::error("lyra::EntityComponentSystem::removeComponent(): Entity with ID: {} was empty!", entityId);
	}

	template <class Ty> constexpr bool containsComponent(object_id e) const {
		return m_lookup.at(e)->contains<Ty>();
	}

	template <class Ty> constexpr Ty& component(object_id e) noexcept {
		return m_lookup.at(e)->component<Ty>(e);
	}
	template <class Ty> constexpr const Ty& component(object_id e) const noexcept {
		return m_lookup.at(e)->component<Ty>(e);
	}


	/*
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
	*/

private:
	archetypes m_archetypes;
	entity_to_archetype m_lookup;

	EntityManager m_entities;

	template <class Ty> Archetype* addOrFindSuperset(Archetype* baseArchetype) {
		auto& edge = baseArchetype->template edge<Ty>();
		auto archetype = edge.superset;

		if (!archetype) {
			auto hash = Archetype::superHash<Ty>(*baseArchetype);
			auto it = m_archetypes.find(hash);

			if (it != m_archetypes.end())
				archetype = it->get();
			else
				archetype = m_archetypes.emplace(archetype::create(Archetype::createSuper<Ty>(*baseArchetype, hash))).first->get();
			
			edge.superset = archetype;
		}

 		return archetype;
	}
	template <class Ty> Archetype* addOrFindSubset(Archetype* baseArchetype) {
		auto& edge = baseArchetype->template edge<Ty>();
		auto archetype = edge.subset;

		if (!archetype) {
			auto hash = Archetype::subHash<Ty>(*baseArchetype);
			auto it = m_archetypes.find(hash);

			if (it != m_archetypes.end())
				archetype = it->get();
			else
				archetype = m_archetypes.emplace(archetype::create(Archetype::createSuper<Ty>(*baseArchetype, hash))).first->get();
			
			edge.subset = archetype;
		}

 		return archetype;
	}
};

namespace ecs {

extern EntityComponentSystem* globalECS;

} // namespace ecs

} // namespace lyra
