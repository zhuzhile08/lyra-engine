#include <ECS/World/Archetype.h>

#include <Common/Logger.h>

namespace lyra {

namespace ecs {

size_type Archetype::superHash(const Archetype& archetype, type_id typeId) {
	auto hash = archetype.m_components.size() + 1;

	auto inserted = false;

	for (const auto& component : archetype.m_components) {
		if (component.first > typeId) {
			inserted = true;
			hash ^= reinterpret_cast<uintptr>(typeId) + 0x9e3779b9 + (hash << 6) + (hash >> 2);
		}

		hash ^= reinterpret_cast<uintptr>(component.first) + 0x9e3779b9 + (hash << 6) + (hash >> 2);
	}
	if (!inserted) hash ^= reinterpret_cast<uintptr>(typeId) + 0x9e3779b9 + (hash << 6) + (hash >> 2);

	return hash;
}

size_type Archetype::subHash(const Archetype& archetype, type_id typeId) {
	auto hash = archetype.m_components.size() - 1;

	for (const auto& component : archetype.m_components)
		if (component.first != typeId)
			hash ^= reinterpret_cast<uintptr>(component.first) + 0x9e3779b9 + (hash << 6) + (hash >> 2);

	return hash;
}

void Archetype::eraseEntity(object_id entityId) {
	auto it = m_entities.find(entityId);

	if (it != m_entities.end()) {
		auto index = (it - m_entities.begin()); // not possible with std::unordered_map
		m_entities.erase(it);

		for (auto& component : m_components) component.second.removeComponent(index);
	} else log::error("lyra::EnitityComponentSystem::Archetype::eraseEntity(): Archetype did not contain entity with ID: {}!", entityId);
}

} // namespace ecs

} // namespace lyra
