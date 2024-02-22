#include <Entity/ECS.h>

#include <Common/Logger.h>
#include <Common/UniquePointer.h>

#include <Entity/Component.h>
#include <Entity/Entity.h>

#include <unordered_map>
#include <algorithm>

namespace lyra {

namespace {

class EntityComponentSystem {
private:
	class EntityManager {
	public:
		objectid uniqueID() {
			if (m_unused.empty()) {
				auto s = m_entities.size();
				m_entities.resize(s + 1);
				return s;
			} else {
				auto id = m_unused.back();
				m_unused.pop_back();
				return id;
			}
		}
		void addEntity(Entity* e) {
			m_entities[e->id()] = e;
		}

		Entity* entity(objectid id) {
			return m_entities[id];
		}

		void returnEntity(objectid id) {
			m_unused.push_back(id);
			m_entities[id] = nullptr;
		}

	private:
		std::vector<Entity*> m_entities;
		std::vector<objectid> m_unused;
	};

	class ComponentManager {
	public:
		ComponentManager() = default;
		ComponentManager(size_t size) : m_size(size) { }

		objectid allocateComponent(void*& memory) {
			if (m_unused.empty()) {
				auto s = m_data.size();
				m_data.resize(s + m_size);
				memory = &m_data[s];
				return s / m_size;
			} else {
				auto i = m_unused.back();
				memory = &m_data[i * m_size];
				m_unused.pop_back();
				return i;
			}
		}

		void returnComponent(objectid id) {
			m_unused.push_back(id);
		}

		void* componentMemory(objectid id) {
			return &m_data[id * m_size];
		}
		const void* componentMemory(objectid id) const {
			return &m_data[id * m_size];
		}

	private:
		std::vector<char> m_data;
		std::vector<objectid> m_unused;

		size_t m_size;
	};

public:
	// component functions
	void* addComponent(std::type_index type, size_t size, objectid e) {
		auto& alloc = componentManagers.try_emplace(type, size).first->second;
		void* data;
		auto id = alloc.allocateComponent(data);

		lookup.resize(std::max(size_t(e + 1), size_t(lookup.size() + 1)));
		lookup[e].emplace(type, id);

		return data;
	}	
	void removeComponent(std::type_index type, objectid e) {
		auto& entity = lookup.at(e);
		auto it = entity.find(type);

		componentManagers.at(type).returnComponent(it->second);

		entity.erase(it);
	} 
	void removeAllComponents(objectid e) {
		for (auto& component : lookup.at(e)) {
			componentManagers.at(component.first).returnComponent(component.second);
		}

		lookup.at(e).clear();
	}

	void* component(std::type_index type, objectid e) {
		return componentManagers.at(type).componentMemory(lookup.at(e).at(type));
	}
	const void* component(std::type_index type, objectid e) const {
		return componentManagers.at(type).componentMemory(lookup.at(e).at(type));
	}
	bool containsComponent(std::type_index type, objectid e) const {
		return lookup.at(e).contains(type);
	}


	// find functions
	std::vector<Entity*> findEntities(std::initializer_list<std::type_index>&& types) {
		std::vector<Entity*> r;

		for (objectid i = 0; i < lookup.size(); i++) {
			if (std::all_of(
				types.begin(), 
				types.end(), 
				[this, i](const std::type_index& key) {
					return lookup[i].contains(key);
				}
			)) r.push_back(entityManager.entity(i));
		}

		return r;
	}

	// system 

	EntityManager entityManager;
	std::unordered_map<std::type_index, ComponentManager> componentManagers;
	
	std::vector<std::unordered_map<std::type_index, objectid>> lookup;
};

static EntityComponentSystem* globalECS;

}


void initECS() {
	if (globalECS) {
		log::error("initECS(): The entity component system is already initialized!");
		return;
	}

	globalECS = new EntityComponentSystem();
}


Entity::Entity(std::string_view name, Entity* parent) : 
	Node<Entity>(name, parent), 
	m_id(globalECS->entityManager.uniqueID()) { 
	globalECS->entityManager.addEntity(this);
}

Entity::~Entity() {
	auto& lookup = globalECS->lookup.at(m_id);

	for (const auto& [type, id] : lookup) {
		globalECS->componentManagers.at(type).returnComponent(id);
	}
	lookup.clear();
	globalECS->entityManager.returnEntity(m_id);
}


namespace ecs {

Entity* entity(objectid id) {
	return globalECS->entityManager.entity(id);
}

void* addComponent(std::type_index type, size_t size, objectid e) {
	return globalECS->addComponent(type, size, e);
}
void removeComponent(std::type_index type, objectid e) {
	globalECS->removeComponent(type, e);
}
void removeAllComponents(objectid e) {
	globalECS->removeAllComponents(e);
}

void* component(std::type_index type, objectid e) {
	return globalECS->component(type, e);
}
const void* c_component(std::type_index type, objectid e) {
	return globalECS->component(type, e);
}
bool containsComponent(std::type_index type, objectid e) {
	return globalECS->containsComponent(type, e);
}

std::vector<Entity*> findEntities(std::initializer_list<std::type_index>&& types) {
	return globalECS->findEntities(std::move(types));
}

} // namespace ecs

} // namespace lyra