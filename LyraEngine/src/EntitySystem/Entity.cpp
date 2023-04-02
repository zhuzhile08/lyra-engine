#include <EntitySystem/Entity.h>

#include <EntitySystem/Script.h>
#include <EntitySystem/Transform.h>

namespace lyra {

Entity::Entity(
    std::string_view name, 
    Entity* parent, 
    Script* script, 
    const uint32& tag, 
    const bool& visible, 
    const bool& constant
) : Node<Entity>(name, parent), m_tag(tag), m_visible(visible), m_script(script), m_constant(constant), m_components(31) { 
    add_component<Transform>();
    if (m_script) m_script->init(); 
}

Entity::Entity(
    Entity&& parent,
    std::string_view name, 
    Script* script, 
    const uint32& tag, 
    const bool& visible, 
    const bool& constant
) : Node<Entity>(name, parent), m_tag(tag), m_visible(visible), m_script(script), m_constant(constant), m_components(31) { 
    add_component<Transform>();
    if (m_script) m_script->init(); 
}

Entity::Entity(Entity&& entityData)
     : m_tag(entityData.m_tag), m_visible(entityData.m_visible), m_script(std::move(entityData.m_script)), m_components(std::move(entityData.m_components)) { 
    if (m_script) m_script->init();
}

void Entity::update() {
    m_script->update();
}

} // namespace lyra
