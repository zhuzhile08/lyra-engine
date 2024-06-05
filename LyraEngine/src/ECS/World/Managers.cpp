#include <ECS/World/Managers.h>

#include <ECS/Entity.h>
#include <ECS/System.h>
#include <ECS/World/Archetype.h>
#include <ECS/World/World.h>

namespace lyra {

namespace ecs {

void EntityManager::insert(pointer entity) {
	auto& archetype = m_world->m_archetypes.front();

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

Archetype* const& EntityManager::archetype(object_id entityId) const {
	return m_lookup.at(entityId);
}


void SystemManager::insert(pointer object) {
	m_lookup.emplace(object, object->generateHash());
}

void SystemManager::erase(object_id systemId) {
	m_unused.pushBack(systemId);
	m_lookup.erase(systemId);
}

size_type SystemManager::hash(object_id systemId) const {
	return m_lookup.at(systemId);
}

void SystemManager::update() {
	for (auto& system : m_lookup) system.first->run();
}

} // namespace ecs

} // namespace lyra
