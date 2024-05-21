/*************************
 * @file Archetype.h
 * @author Zhile Zhu (zhuzhile08@gmail.com)
 * 
 * @brief ECS archetype and component memory allocator
 * 
 * @date 2024-01-31
 * 
 * @copyright Copyright (c) 2024
 *************************/

#pragma once

#include <Common/Common.h>
#include <Common/Utility.h>
#include <Common/UniquePointer.h>
#include <Common/Hash.h>
#include <Common/UnorderedSparseMap.h>
#include <Common/UnorderedSparseSet.h>

namespace lyra {

namespace ecs {

class Archetype {
private:
	class ComponentAllocator {
	private:
		class BasicMemory {
		public:
			virtual ~BasicMemory() { }

			virtual bool emptyComponent() const noexcept = 0;

			virtual void* emplaceBack(void*) = 0;
			virtual void removeComponent(size_type) = 0;

			virtual void* componentData(size_type) = 0;
			virtual const void* componentData(size_type) const = 0;

			virtual size_type count() const noexcept = 0;

			virtual void* at(size_type) noexcept = 0;
			virtual const void* at(size_type) const noexcept = 0;

			virtual UniquePointer<BasicMemory> copyType() const = 0;
		};

		template <class Ty> class Memory : public BasicMemory {
			using value_type = Ty;
			using memory = std::conditional_t<std::is_empty_v<value_type>, Ty, Vector<Ty>>; // tiny memory optimization

			bool emptyComponent() const noexcept override {
				return std::is_empty_v<value_type>;
			}

			void* emplaceBack(void* data) override {
				if constexpr (std::is_empty_v<value_type>) return &m_memory;
				else return &m_memory.emplaceBack(std::move(*static_cast<Ty*>(data)));
			}
			void removeComponent(size_type index) override {
				if constexpr (!std::is_empty_v<value_type>) {
					m_memory[index] = std::move(m_memory.back());
					m_memory.popBack();
				}
			}

			void* componentData(size_type index) override {
				if constexpr (std::is_empty_v<value_type>) return &m_memory;
				else return &m_memory[index];
			}
			const void* componentData(size_type index) const override {
				if constexpr (std::is_empty_v<value_type>) return &m_memory;
				else return &m_memory[index];
			}

			size_type count() const noexcept override {
				if constexpr (std::is_empty_v<value_type>) return 1;
				else return m_memory.size();
			}

			void* at(size_type index) noexcept override {
				if constexpr (std::is_empty_v<value_type>) return &m_memory;
				else return &m_memory.at(index);
			}
			const void* at(size_type index) const noexcept override {
				if constexpr (std::is_empty_v<value_type>) return &m_memory;
				else return &m_memory.at(index);
			}

			UniquePointer<BasicMemory> copyType() const override {
				return UniquePointer<Memory>::create();
			}
		
		private:
			memory m_memory;
		};

		ComponentAllocator() = default; // cursed

	public:
		ComponentAllocator(ComponentAllocator&&) = default;
		ComponentAllocator(const ComponentAllocator& other) : m_memory(other.m_memory->copyType()) { }

		template <class Ty> static ComponentAllocator create() {
			ComponentAllocator a;
			a.m_memory = UniquePointer<Memory<Ty>>::create();
			return a;
		}

		bool emptyComponent() {
			return m_memory->emptyComponent();
		}

		template <class Ty> void* emplaceBack(Ty&& component) {
			Ty cmp = std::move(component);
			return m_memory->emplaceBack(&cmp);
		}
		void* emplaceBackData(void* component) {
			return m_memory->emplaceBack(component);
		}
		void removeComponent(size_type index) {
			m_memory->removeComponent(index);
		}

		template <class Ty> Ty* component(size_type index) {
			return static_cast<Ty*>(m_memory->componentData(index));
		}
		template <class Ty> const Ty* component(size_type index) const {
			return static_cast<Ty*>(m_memory->componentData(index));
		}
		void* componentData(size_type index) {
			return m_memory->componentData(index);
		}
		const void* componentData(size_type index) const {
			return m_memory->componentData(index);
		}

		size_type count() const noexcept {
			return m_memory->count();
		}

		template <class Ty> Ty& at(size_type index) noexcept {
			return *static_cast<Ty*>(m_memory->at(index));
		}
		template <class Ty> const Ty& at(size_type index) const noexcept {
			return *static_cast<Ty*>(m_memory->at(index));
		}

	private:
		UniquePointer<BasicMemory> m_memory;
	};

	struct Edge { 
	public:
		Archetype* superset { };
		Archetype* subset { };
	};

	CUSTOM_HASHER(Hasher, const UniquePointer<Archetype>&, size_type, static_cast<size_type>, ->m_hash)
	CUSTOM_EQUAL(Equal, const UniquePointer<Archetype>&, size_type, ->m_hash)

	using component_alloc = ComponentAllocator;
	using components = UnorderedSparseMap<type_id, component_alloc>;

	using edges = UnorderedSparseMap<type_id, Edge>;
	using entities = UnorderedSparseSet<object_id>;
	
public:
	constexpr Archetype() = default;
	constexpr Archetype(Archetype&&) = default;

	template <class Container> static size_type generateHash(const Container& container) {
		auto hash = container.size() + 1;

		for (const auto& component : container) {
			hash ^= reinterpret_cast<uintptr>(component) + 0x9e3779b9 + (hash << 6) + (hash >> 2);
		}

		return hash;
	}

	static size_type superHash(const Archetype& archetype, type_id typeId);
	static size_type subHash(const Archetype& archetype, type_id typeId);

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
		ASSERT(!archetype.m_components.empty(), "lyra::ecs::World::Archetype::createSub(): Cannot create subset archetype of empty archetype!");

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

	template <class Ty, class... Args> Ty& insertEntityFromSub(object_id entityId, Archetype& subset, Args&&... args) {
		m_entities.emplace(entityId);

		auto& c = *static_cast<Ty*>(m_components.at(typeId<Ty>()).emplaceBack(Ty(std::forward<Args>(args)...)));

		auto entityIndex = (subset.m_entities.find(entityId) - subset.m_entities.begin());

		for (auto& component : subset.m_components) {
			m_components.at(component.first).emplaceBackData(
				component.second.componentData(entityIndex)
			);
		}

		subset.eraseEntity(entityId);

		return c;
	}
	template <class Ty> Ty insertEntityFromSuper(object_id entityId, Archetype& superset) {
		m_entities.emplace(entityId);

		auto entityIndex = (superset.m_entities.find(entityId) - superset.m_entities.begin());

		auto id = typeId<Ty>();

		for (auto& component : superset.m_components)
			if (component.first != id) 
				m_components.at(component.first).emplaceBackData(component.second.componentData(entityIndex));

		superset.eraseEntity(entityId);
	}

	void eraseEntity(object_id entityId);

	template <class Ty> Ty& component(object_id entityId) {
		return *m_components.at(typeId<Ty>()).template component<Ty>(m_entities.at(entityId));
	}
	template <class Ty> const Ty& component(object_id entityId) const {
		return *m_components.at(typeId<Ty>()).template component<Ty>(m_entities.at(entityId));
	}

	template <class Ty> bool contains() const {
		return m_components.contains(typeId<Ty>());
	}

	template <class Ty> const Edge& edge() const {
		return m_edges.at(typeId<Ty>());
	}
	template <class Ty> Edge& edge() {
		return m_edges[typeId<Ty>()];
	}

	template <class... Types, class Callable> void each(Callable& system) {
		for (auto i = m_entities.size(); i > 0; i++) system(*m_components.at(typeId<Types>()).template at<Types>(i)...);

		for (auto& edge : m_edges) {
			auto superset = edge.second.superset;
			if (superset) superset->each(system);
		}
	}
	template <class... Types, class Callable> void each(const Callable& system) const {
		for (auto i = m_entities.size(); i > 0; i++) system(*m_components.at(typeId<Types>()).template at<Types>(i)...);

		for (const auto& edge : m_edges) {
			auto superset = edge.second.superset;
			if (superset) superset->each(system);
		}
	}

private:
	components m_components;
	entities m_entities;

	edges m_edges;
	size_type m_hash = 0;

	friend class Hasher;
	friend class Equal;
	friend class World;
	friend class EntityManager;
};

} // namespace ecs

} // namespace lyra
