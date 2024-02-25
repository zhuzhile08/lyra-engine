#include <Entity/ECS.h>

#include <Common/Logger.h>
#include <Common/UniquePointer.h>
#include <Common/Vector.h>

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
				m_unused.popBack();
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
			m_unused.pushBack(id);
			m_entities[id] = nullptr;
		}

	private:
		Vector<Entity*> m_entities;
		Vector<objectid> m_unused;
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
				m_unused.popBack();
				return i + 1;
			}
		}

		void returnComponent(objectid id) {
			m_unused.pushBack(id);
		}

		void* componentMemory(objectid id) {
			return &m_data[id * m_size - id];
		}
		const void* componentMemory(objectid id) const {
			return &m_data[id * m_size - id];
		}

	private:
		Vector<char> m_data;
		Vector<objectid> m_unused;

		size_t m_size;
	};

public:
	// component functions
	void* addComponent(objectid type, size_t size, objectid e) {
		if ((componentManagers.size() + 1) > type) {
			componentManagers.resize(type);
			componentManagers.emplaceBack(size);
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
	Vector<Entity*> findEntities(std::initializer_list<objectid>&& types) {
		Vector<Entity*> r;

		for (objectid i = 0; i < lookup.size(); i++) {
			bool found = true;
			auto& l = lookup[i];

			for (objectid j = 0; j < types.size(); j++) {
				if (!((l.size() > j) && (l[j] != 0))) {
					found = false;
					break;
				}
			}

			if (found) r.pushBack(entityManager.entity(i));
		}

		return r;
	}

	// systems function
	void executeSystem(std::initializer_list<objectid>&& types, void (*system)(void*, Entity*), void* c) {
		for (objectid i = 0; i < lookup.size(); i++) {
			bool found = true;
			auto& l = lookup[i];

			for (objectid j = 0; j < types.size(); j++) {
				if (!((l.size() > j) && (l[j] != 0))) {
					found = false;
					break;
				}
			}

			if (found) system(c, entityManager.entity(i));
		}
	}

	EntityManager entityManager;
	Vector<ComponentManager> componentManagers;
	
	Vector<Vector<objectid>> lookup; // horizontal: component mappings, vertical: entity mappings

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

Vector<Entity*> findEntities(std::initializer_list<objectid>&& types) {
	return globalECS->findEntities(std::move(types));
}
void executeSystem(std::initializer_list<objectid>&& types, void (*system)(void*, Entity*), void* c) {
	globalECS->executeSystem(std::move(types), system, c);
}

objectid uniqueID() {
	return globalECS->uniqueID++;
}

} // namespace ecs

} // namespace lyra
