/*************************
 * @file Entity.h
 * @author Zhile Zhu (zhuzhile08@gmail.com)
 * 
 * @brief game object class
 * 
 * @date 2022-24-7
 * 
 * @copyright Copyright (c) 2022
*************************/

#pragma once

#include <Lyra/Lyra.h>

#include <type_traits>
#include <unordered_map>
#include <Common/SmartPointer.h>
#include <Common/Node.h>

namespace lyra {

/**
 * @brief a base class for all components
 */
class ComponentBase { 
public:
	virtual void update() = 0;

	/**
	 * @brief get the entity which owns this component
	 * 
	 * @return lyra::Entity* 
	 */
	Entity* entity() { return m_entity; }
protected:
	Entity* m_entity;
};

template <class Ty> concept DerivedComponentType = std::is_base_of_v<ComponentBase, Ty>;

/**
 * @brief Implementation of a basic entity class
 */
class Entity : public Node<Entity> {
public:
	// enum containing all types of components
	enum class ComponentType {
		TYPE_TRANSFORM = 0U,
		TYPE_MESH_RENDERER,
		TYPE_TEXT_RENDERER,
		TYPE_RIGIDBODY,
		TYPE_COLLIDER,
		TYPE_BOX_COLLIDER,
		TYPE_SPHERE_COLLIDER,
		TYPE_CAPSULE_COLLIDER,
		TYPE_TAPERED_CAPSULE_COLLIDER,
		TYPE_CYLINDER_COLLIDER,
		TYPE_MESH_COLLIDER,
		TYPE_JOINT,
		TYPE_CLOTH,
		TYPE_RAYCAST,
		TYPE_CAMERA,
		TYPE_POINT_LIGHT,
		TYPE_DIRECTIONAL_LIGHT,
		TYPE_SPOT_LIGHT,
		TYPE_AMBIENT_LIGHT,
		TYPE_SKYBOX,
		TYPE_MOVIE_PLAYER,
		TYPE_PARTICLE_SYSTEM,
		TYPE_LINE_RENDERER,
		TYPE_ANIMATOR,
		TYPE_AUDIO_SOURCE,
		TYPE_AUDIO_LISTENER,
		TYPE_AUDIO_FILTER
	};

private:
	/**
	 * @brief cast a type of entity to the entity type enum
	 * 
	 * @tparam Ty type of entity
	 * 
	 * @return constexpr lyra::Node::ComponentType 
	 */
	template <DerivedComponentType Ty> static constexpr ComponentType cast_enum_type() {
		if constexpr (std::is_same_v<Ty, Transform>) return ComponentType::TYPE_TRANSFORM;
		else if constexpr (std::is_same_v<Ty, MeshRenderer>) return ComponentType::TYPE_MESH_RENDERER;
		else if constexpr (std::is_same_v<Ty, TextRenderer>) return ComponentType::TYPE_TEXT_RENDERER;
		else if constexpr (std::is_same_v<Ty, Rigidbody>) return ComponentType::TYPE_RIGIDBODY;
		else if constexpr (std::is_same_v<Ty, Collider>) return ComponentType::TYPE_COLLIDER;
		else if constexpr (std::is_same_v<Ty, BoxCollider>) return ComponentType::TYPE_BOX_COLLIDER;
		else if constexpr (std::is_same_v<Ty, SphereCollider>) return ComponentType::TYPE_SPHERE_COLLIDER;
		else if constexpr (std::is_same_v<Ty, CapsuleCollider>) return ComponentType::TYPE_CAPSULE_COLLIDER;
		else if constexpr (std::is_same_v<Ty, TaperedCapsuleCollider>) return ComponentType::TYPE_TAPERED_CAPSULE_COLLIDER;
		else if constexpr (std::is_same_v<Ty, CylinderCollider>) return ComponentType::TYPE_CYLINDER_COLLIDER;
		else if constexpr (std::is_same_v<Ty, MeshCollider>) return ComponentType::TYPE_MESH_COLLIDER;
		else if constexpr (std::is_same_v<Ty, Joint>) return ComponentType::TYPE_JOINT;
		else if constexpr (std::is_same_v<Ty, Cloth>) return ComponentType::TYPE_CLOTH;
		else if constexpr (std::is_same_v<Ty, Raycast>) return ComponentType::TYPE_RAYCAST;
		else if constexpr (std::is_same_v<Ty, Camera>) return ComponentType::TYPE_CAMERA;
		else if constexpr (std::is_same_v<Ty, PointLight>) return ComponentType::TYPE_POINT_LIGHT;
		else if constexpr (std::is_same_v<Ty, DirectionalLight>) return ComponentType::TYPE_DIRECTIONAL_LIGHT;
		else if constexpr (std::is_same_v<Ty, SpotLight>) return ComponentType::TYPE_SPOT_LIGHT;
		else if constexpr (std::is_same_v<Ty, AmbientLight>) return ComponentType::TYPE_AMBIENT_LIGHT;
		else if constexpr (std::is_same_v<Ty, Skybox>) return ComponentType::TYPE_SKYBOX;
		else if constexpr (std::is_same_v<Ty, MoviePlayer>) return ComponentType::TYPE_MOVIE_PLAYER;
		else if constexpr (std::is_same_v<Ty, ParticleSystem>) return ComponentType::TYPE_PARTICLE_SYSTEM;
		else if constexpr (std::is_same_v<Ty, LineRenderer>) return ComponentType::TYPE_LINE_RENDERER;
		else if constexpr (std::is_same_v<Ty, Animator>) return ComponentType::TYPE_ANIMATOR;
		else if constexpr (std::is_same_v<Ty, AudioSource>) return ComponentType::TYPE_AUDIO_SOURCE;
		else if constexpr (std::is_same_v<Ty, AudioListener>) return ComponentType::TYPE_AUDIO_LISTENER;
		else if constexpr (std::is_same_v<Ty, AudioFilter>) return ComponentType::TYPE_AUDIO_FILTER;
	}
public:
	Entity() = default;
	/**
	 * @brief constuct a new entity
	 * 
	 * @param name name of the object
	 * @param parent parent of the object
	 * @param script script pointer
	 * @param tag tag(s) applied to the object
	 * @param visible visiblility option
	 * @param constant constant option
	 */
	Entity(
		std::string_view name = "Entity", 
		Entity* parent = nullptr, 
		Script* script = nullptr, 
		const uint32& tag = 0, 
		const bool& visible = true, 
		const bool& constant = false
	);
	/**
	 * @brief constuct a new entity
	 * 
	 * @param name name of the object
	 * @param parent parent of the object
	 * @param script script pointer
	 * @param tag tag(s) applied to the object
	 * @param visible visiblility option
	 * @param constant constant option
	 */
	Entity(
		Entity&& parent, 
		std::string_view name = "Entity", 
		Script* script = nullptr, 
		const uint32& tag = 0, 
		const bool& visible = true, 
		const bool& constant = false
	);
	/**
	 * @brief move constructor for a structure holding the data for an entity
	 * 
	 * @param entityData structure to move data from
	 */
	Entity(Entity&& entityData);
	
	/**
	 * @brief update function, updates the script
	 */
	void update();

	/**
	 * @brief add a component of a certain type
	 * 
	 * @tparam Ty type of component to add
	 * @tparam Args arguments to construct that component
	 */
	template <DerivedComponentType Ty, class... Args> void add_component(Args... args) {
		m_components.emplace(cast_enum_type<Ty>(), SmartPointer<Ty>::create(std::forward<Args>(args)...));
	}
	/**
	 * @brief add a component of a certain type
	 * 
	 * @tparam Ty type of component to add
	 * @tparam Args arguments to construct that component
	 */
	template <DerivedComponentType Ty> void add_component(SmartPointer<Ty>&& component) {
		m_components.emplace(cast_enum_type<Ty>(), std::move(component));
	}

	/**
	 * @brief get a component by its type
	 * 
	 * @tparam Ty type of component to get
	 * 
	 * @return Ty 
	 */
	template <DerivedComponentType Ty> NODISCARD Ty* component() {
		return static_cast<Ty*>(m_components[cast_enum_type<Ty>()]);
	}
	/**
	 * @brief get a component by its type
	 * 
	 * @tparam Ty type of component to get
	 * 
	 * @return Ty 
	 */
	template <DerivedComponentType Ty> NODISCARD Ty* component() const {
		return static_cast<Ty*>(m_components.at(cast_enum_type<Ty>()));
	}

	/**
	 * @brief get the tag(s) of the entity
	 * 
	 * @return uint32_t
	 */
	NODISCARD constexpr uint32 tag() const noexcept {
		return m_tag;
	}
	/**
	 * @brief get the visibility of the entity
	 * 
	 * @return uint32
	 */
	NODISCARD constexpr bool visible() const noexcept {
		return m_visible;
	}
	/**
	 * @brief get the constantity of the entity (is constantity a word?)
	 * 
	 * @return uint32
	 */
	NODISCARD constexpr bool constant() const noexcept {
		return m_constant;
	}

private:
	uint32 m_tag;
	bool m_visible;
	bool m_constant;

	SmartPointer<Script> m_script;
	std::unordered_map<ComponentType, SmartPointer<ComponentBase>> m_components;
};

} // namespace lyra
