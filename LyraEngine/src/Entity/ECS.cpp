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
				return s / m_size + 1;
			} else {
				auto i = m_unused.back();
				memory = &m_data[i * m_size];
				m_unused.pop_back();
				return i + 1;
			}
		}

		void returnComponent(objectid id) {
			m_unused.push_back(id);
		}

		void* componentMemory(objectid id) {
			return &m_data[id * m_size - id];
		}
		const void* componentMemory(objectid id) const {
			return &m_data[id * m_size - id];
		}

	private:
		std::vector<char> m_data;
		std::vector<objectid> m_unused;

		size_t m_size;
	};

public:
	// component functions
	void* addComponent(objectid type, size_t size, objectid e) {
		if ((componentManagers.size() + 1) > type) {
			componentManagers.resize(type);
			componentManagers.emplace_back(size);
		}

		auto& alloc = componentManagers[type];
		void* data;

		lookup.resize(std::max(size_t(e + 1), size_t(lookup.size() + 1)));
		lookup[e].resize(std::max(size_t(type + 1), size_t(lookup[e].size() + 1)));
		lookup[e][type] = alloc.allocateComponent(data);

		return data;
	}	
	void removeComponent(objectid type, objectid e) {
		auto& entity = lookup[e];

		componentManagers[type].returnComponent(entity[type]);

		entity[type] = 0;
	} 
	void removeAllComponents(objectid e) {
		for (objectid i = 0; i < lookup[e].size(); i++) {
			componentManagers[i].returnComponent(lookup[e][i]);
		}

		lookup.at(e).clear();
	}

	void* component(objectid type, objectid e) {
		if (containsComponent(type, e)) {
			return componentManagers[type].componentMemory(lookup[e][type]);
		} else {
			return nullptr;
		}
	}
	const void* component(objectid type, objectid e) const {
		if (containsComponent(type, e)) {
			return componentManagers[type].componentMemory(lookup[e][type]);
		} else {
			return nullptr;
		}
	}
	bool containsComponent(objectid type, objectid e) const {
		return (lookup[e].size() > type) && (lookup[e][type] != 0);
	}


	// find function
	std::vector<Entity*> findEntities(std::initializer_list<objectid>&& types) {
		std::vector<Entity*> r;

		for (objectid i = 0; i < lookup.size(); i++) {
			if (std::all_of(
				types.begin(), 
				types.end(), 
				[this, i](objectid key) {
					return (lookup[i].size() > key) && (lookup[i][key] != 0);
				}
			)) r.push_back(entityManager.entity(i));
		}

		return r;
	}
	// systems function
	void executeSystem(std::initializer_list<objectid>&& types, const Function<void(Entity*)>& system) {
		for (objectid i = 0; i < lookup.size(); i++) {
			if (std::all_of(
				types.begin(), 
				types.end(), 
				[this, i](objectid key) {
					return (lookup[i].size() > key) && (lookup[i][key] != 0);
				}
			)) system(entityManager.entity(i));
		}
	}

	EntityManager entityManager;
	std::vector<ComponentManager> componentManagers;
	
	std::vector<std::vector<objectid>> lookup;

	objectid uniqueID;
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
	globalECS->removeAllComponents(m_id);
	globalECS->entityManager.returnEntity(m_id);
}


namespace ecs {

Entity* entity(objectid id) {
	return globalECS->entityManager.entity(id);
}

void* addComponent(objectid type, size_t size, objectid e) {
	return globalECS->addComponent(type, size, e);
}
void removeComponent(objectid type, objectid e) {
	globalECS->removeComponent(type, e);
}
void removeAllComponents(objectid e) {
	globalECS->removeAllComponents(e);
}

void* component(objectid type, objectid e) {
	return globalECS->component(type, e);
}
const void* c_component(objectid type, objectid e) {
	return globalECS->component(type, e);
}
bool containsComponent(objectid type, objectid e) {
	return globalECS->containsComponent(type, e);
}

std::vector<Entity*> findEntities(std::initializer_list<objectid>&& types) {
	return globalECS->findEntities(std::move(types));
}
void executeSystem(std::initializer_list<objectid>&& types, const Function<void(Entity*)>& system) {
	globalECS->executeSystem(std::move(types), system);
}

objectid uniqueID() {
	return globalECS->uniqueID++;
}

} // namespace ecs

} // namespace lyra
