/*************************
 * @file World.h
 * @author Zhile Zhu (zhuzhile08@gmail.com)
 * 
 * @brief Includes all headers of the world class
 * 
 * @date 2024-05-19
 * 
 * @copyright Copyright (c) 2024
 *************************/

#pragma once

#include <Common/Common.h>
#include <LSD/UniquePointer.h>
#include <LSD/Hash.h>
#include <LSD/UnorderedSparseSet.h>

#include <ECS/World/Archetype.h>
#include <ECS/World/Managers.h>

namespace lyra {

namespace ecs {

class World {
public:
	using archetype = lsd::UniquePointer<Archetype>;
	using archetypes = lsd::UnorderedSparseSet<archetype, Archetype::Hasher, Archetype::Equal>;

	World() : m_entities(this), m_systems(this) { 
		m_archetypes.emplace(archetype::create()); 
	}


	// entity functions

	void insertEntity(Entity* entity) {
		m_entities.insert(entity);
	}
	void eraseEntity(object_id entityId) {
		m_entities.erase(entityId);
	}
	void clearEntity(object_id entityId) {
		m_entities.clear(entityId);
	}


	// component functions

	template <class Ty, class... Args> Ty& addComponent(object_id entityId, Args&&... args) {
		auto& base = m_entities.archetype(entityId);
		Archetype* archetype = addOrFindSuperset<Ty>(base);

		return archetype->insertEntityFromSub<Ty>(entityId, *std::exchange(base, archetype), std::forward<Args>(args)...);
	}
	template <class Ty> void removeComponent(object_id entityId) {
		auto& base = m_entities.archetype(entityId);
		Archetype* archetype = addOrFindSubset<Ty>(base);

		archetype->insertEntityFromSuper<Ty>(entityId, *std::exchange(base, archetype));
	}

	template <class Ty> bool containsComponent(object_id entityId) const {
		return m_entities.archetype(entityId)->contains<Ty>();
	}

	template <class Ty> Ty& component(object_id entityId) {
		return m_entities.archetype(entityId)->component<Ty>(entityId);
	}
	template <class Ty> const Ty& component(object_id entityId) const {
		return m_entities.archetype(entityId)->component<Ty>(entityId);
	}


	// systems functions

	void insertSystem(BasicSystem* system) {
		m_systems.insert(system);
	}
	void eraseSystem(object_id systemId) {
		m_systems.erase(systemId);
	}

	Archetype* systemArchetype(object_id systemId) {
		auto archetype = m_archetypes.find(m_systems.hash(systemId));

		if (archetype != m_archetypes.end()) return archetype->get();
		else return nullptr;
	}


	// general update function

	void update() {
		m_systems.update();
	}

private:
	archetypes m_archetypes;

	EntityManager m_entities;
	SystemManager m_systems;

	template <class Ty> Archetype* addOrFindSuperset(Archetype* baseArchetype) {
		auto& edge = baseArchetype->template edge<Ty>();
		auto archetype = edge.superset;

		if (!archetype) {
			auto hash = Archetype::superHash(*baseArchetype, lsd::typeId<Ty>());
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
			auto hash = Archetype::subHash(*baseArchetype, lsd::typeId<Ty>());
			auto it = m_archetypes.find(hash);

			if (it != m_archetypes.end())
				archetype = it->get();
			else
				archetype = m_archetypes.emplace(archetype::create(Archetype::createSuper<Ty>(*baseArchetype, hash))).first->get();
			
			edge.subset = archetype;
		}

		return archetype;
	}

	friend class EntityManager;
	friend class SystemManager;
};

extern World* globalWorld;

} // namespace lyra

} // namespace ecs
