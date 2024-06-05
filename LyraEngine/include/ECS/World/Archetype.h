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
#include <LSD/Utility.h>
#include <LSD/UniquePointer.h>
#include <LSD/Hash.h>
#include <LSD/UnorderedSparseMap.h>
#include <LSD/UnorderedSparseSet.h>

#include <tuple>

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

			virtual void* begin() = 0;
			virtual const void* begin() const = 0;

			virtual size_type count() const noexcept = 0;

			virtual lsd::UniquePointer<BasicMemory> copyType() const = 0;
		};

		template <class Ty> class Memory : public BasicMemory {
			using value_type = Ty;
			using memory = std::conditional_t<std::is_empty_v<value_type>, Ty, lsd::Vector<Ty>>; // tiny memory optimization

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

			void* begin() override {
				if constexpr (std::is_empty_v<value_type>) return &m_memory;
				else return m_memory.begin().get();
			}
			const void* begin() const override {
				if constexpr (std::is_empty_v<value_type>) return &m_memory;
				else return m_memory.begin().get();
			}

			size_type count() const noexcept override {
				if constexpr (std::is_empty_v<value_type>) return 1;
				else return m_memory.size();
			}

			lsd::UniquePointer<BasicMemory> copyType() const override {
				return lsd::UniquePointer<Memory>::create();
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
			a.m_memory = lsd::UniquePointer<Memory<Ty>>::create();
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

		template <class Ty> Ty* begin() noexcept {
			return static_cast<Ty*>(m_memory->begin());
		}
		template <class Ty> const Ty* begin() const noexcept {
			return static_cast<const Ty*>(m_memory->begin());
		}

	private:
		lsd::UniquePointer<BasicMemory> m_memory;
	};

	struct Edge { 
	public:
		Archetype* superset { };
		Archetype* subset { };
	};

	CUSTOM_HASHER(Hasher, const lsd::UniquePointer<Archetype>&, size_type, static_cast<size_type>, ->m_hash)
	CUSTOM_EQUAL(Equal, const lsd::UniquePointer<Archetype>&, size_type, ->m_hash)

	using component_alloc = ComponentAllocator;
	using components = lsd::UnorderedSparseMap<lsd::type_id, component_alloc>;

	using edges = lsd::UnorderedSparseMap<lsd::type_id, Edge>;
	using entities = lsd::UnorderedSparseSet<object_id>;
	
public:
	constexpr Archetype() = default;
	constexpr Archetype(Archetype&&) = default;

	static size_type superHash(const Archetype& archetype, lsd::type_id typeId);
	static size_type subHash(const Archetype& archetype, lsd::type_id typeId);

	template <class Ty> static Archetype createSuper(Archetype& archetype, size_type hash) {
		Archetype a;
		a.m_hash = hash;

		{ // insert component in the proper ordered position
			auto compTypeId = lsd::typeId<Ty>();
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
			auto compTypeId = lsd::typeId<Ty>();
			for (const auto& component : archetype.m_components)
				if (component.first != compTypeId) // insert component if it is not of type Ty
					a.m_components.emplace(component.first, component.second);
		}

		a.edge<Ty>().superset = &archetype;

		return a;
	}

	template <class Ty, class... Args> Ty& insertEntityFromSub(object_id entityId, Archetype& subset, Args&&... args) {
		m_entities.emplace(entityId);

		auto& c = *static_cast<Ty*>(m_components.at(lsd::typeId<Ty>()).emplaceBack(Ty(std::forward<Args>(args)...)));

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

		auto id = lsd::typeId<Ty>();

		for (auto& component : superset.m_components)
			if (component.first != id) 
				m_components.at(component.first).emplaceBackData(component.second.componentData(entityIndex));

		superset.eraseEntity(entityId);
	}

	void eraseEntity(object_id entityId);

	template <class Ty> Ty& component(object_id entityId) {
		return *m_components.at(lsd::typeId<Ty>()).template component<Ty>(m_entities.at(entityId));
	}
	template <class Ty> const Ty& component(object_id entityId) const {
		return *m_components.at(lsd::typeId<Ty>()).template component<Ty>(m_entities.at(entityId));
	}

	template <class Ty> bool contains() const {
		return m_components.contains(lsd::typeId<Ty>());
	}

	template <class Ty> const Edge& edge() const {
		return m_edges.at(lsd::typeId<Ty>());
	}
	template <class Ty> Edge& edge() {
		return m_edges[lsd::typeId<Ty>()];
	}

	template <class... Types, class Callable> void each(Callable& system) {
		for (auto& edge : m_edges) {
			auto superset = edge.second.superset;
			if (superset) superset->each<Types...>(system);
		}

		auto iterators = std::make_tuple((m_components.at(lsd::typeId<std::remove_cv_t<Types>>()).template begin<Types>())...);

		for (auto i = m_entities.size(); i > 0; i--) system((*std::get<Types*>(iterators)++)...);
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
