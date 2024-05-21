#include <ECS/World/Managers.h>

#include <ECS/Entity.h>
#include <ECS/System.h>
#include <ECS/World/Archetype.h>
#include <ECS/World/World.h>

namespace lyra {

namespace ecs {

void EntityManager::insert(pointer entity, Archetype* archetype) {
	entity->m_id = uniqueId();
	m_lookup.emplace(entity, archetype);
	archetype->m_entities.emplace(entity->m_id);
}

void EntityManager::erase(object_id id) {
	m_unused.pushBack(id);
	m_lookup.erase(id);
}

void EntityManager::clear(object_id id) {
	auto& archetype = m_lookup.at(id);
	archetype->eraseEntity(id);
	archetype = m_world->m_archetypes.front();
}

Archetype*& EntityManager::archetype(object_id entityId) {
	return m_lookup.at(entityId);
}

const Archetype* const& EntityManager::archetype(object_id entityId) const {
	return m_lookup.at(entityId);
}


void SystemManager::insert(pointer object, Archetype* archetype) {
	m_lookup.emplace(object, archetype);
}

void SystemManager::erase(object_id id) {
	m_unused.pushBack(id);
	m_lookup.erase(id);
}



} // namespace ecs

} // namespace lyra
