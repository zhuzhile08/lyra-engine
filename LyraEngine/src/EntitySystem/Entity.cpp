#include <EntitySystem/Entity.h>

#include <EntitySystem/Script.h>
#include <EntitySystem/Transform.h>

namespace lyra {

Entity::Entity(
	std::string_view name, 
	Entity* parent, 
	Script* script, 
	uint32 tag, 
	bool visible, 
	bool constant
) : Node<Entity>(this, name, parent), m_tag(tag), m_visible(visible), m_script(script), m_constant(constant), m_components(31) { 
	addComponent<Transform>();
	if (m_script) {
		m_script->node = this;
		m_script->init(); 
	}
}

Entity::Entity(
	Entity&& parent,
	std::string_view name, 
	Script* script, 
	uint32 tag, 
	bool visible, 
	bool constant
) : Node<Entity>(this, name, std::move(parent)), m_tag(tag), m_visible(visible), m_script(script), m_constant(constant), m_components(31) { 
	addComponent<Transform>();
	if (m_script) m_script->init(); {
		m_script->node = this;
		m_script->init(); 
	}
}

Entity::Entity(Entity&& entityData)
	 : m_tag(entityData.m_tag), m_visible(entityData.m_visible), m_script(std::move(entityData.m_script)), m_components(std::move(entityData.m_components)) { }

void Entity::update() {
	m_script->update();
}

} // namespace lyra
