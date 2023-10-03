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

#include <Common/Common.h>

#include <type_traits>
#include <unordered_map>
#include <Common/UniquePointer.h>
#include <Common/Node.h>

namespace lyra {

/**
 * @brief a base class for all components
 */
class ComponentBase { 
public:
	virtual ~ComponentBase() { }

	// virtual void update() = 0; maybe one day

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
		TRANSFORM = 0U,
		MESH_RENDERER,
		TEXT_RENDERER,
		RIGIDBODY,
		COLLIDER,
		BOX_COLLIDER,
		SPHERE_COLLIDER,
		CAPSULE_COLLIDER,
		TAPERED_CAPSULE_COLLIDER,
		CYLINDER_COLLIDER,
		MESH_COLLIDER,
		JOINT,
		CLOTH,
		RAYCAST,
		CAMERA,
		POINT_LIGHT,
		DIRECTIONAL_LIGHT,
		SPOT_LIGHT,
		AMBIENT_LIGHT,
		SKYBOX,
		MOVIE_PLAYER,
		PARTICLE_SYSTEM,
		LINE_RENDERER,
		ANIMATOR,
		AUDIO_SOURCE,
		AUDIO_LISTENER,
		AUDIO_FILTER
	};

private:
	/**
	 * @brief cast a type of entity to the entity type enum
	 * 
	 * @tparam Ty type of entity
	 * 
	 * @return lyra::Node::ComponentType 
	 */
	template <DerivedComponentType Ty> static constexpr ComponentType cast_enum_type() {
		if constexpr (std::is_same_v<Ty, Transform>) return ComponentType::TRANSFORM;
		else if constexpr (std::is_same_v<Ty, MeshRenderer>) return ComponentType::MESH_RENDERER;
		else if constexpr (std::is_same_v<Ty, TextRenderer>) return ComponentType::TEXT_RENDERER;
		else if constexpr (std::is_same_v<Ty, Rigidbody>) return ComponentType::RIGIDBODY;
		else if constexpr (std::is_same_v<Ty, Collider>) return ComponentType::COLLIDER;
		else if constexpr (std::is_same_v<Ty, BoxCollider>) return ComponentType::BOX_COLLIDER;
		else if constexpr (std::is_same_v<Ty, SphereCollider>) return ComponentType::SPHERE_COLLIDER;
		else if constexpr (std::is_same_v<Ty, CapsuleCollider>) return ComponentType::CAPSULE_COLLIDER;
		else if constexpr (std::is_same_v<Ty, TaperedCapsuleCollider>) return ComponentType::TAPERED_CAPSULE_COLLIDER;
		else if constexpr (std::is_same_v<Ty, CylinderCollider>) return ComponentType::CYLINDER_COLLIDER;
		else if constexpr (std::is_same_v<Ty, MeshCollider>) return ComponentType::MESH_COLLIDER;
		else if constexpr (std::is_same_v<Ty, Joint>) return ComponentType::JOINT;
		else if constexpr (std::is_same_v<Ty, Cloth>) return ComponentType::CLOTH;
		else if constexpr (std::is_same_v<Ty, Raycast>) return ComponentType::RAYCAST;
		else if constexpr (std::is_same_v<Ty, Camera>) return ComponentType::CAMERA;
		else if constexpr (std::is_same_v<Ty, PointLight>) return ComponentType::POINT_LIGHT;
		else if constexpr (std::is_same_v<Ty, DirectionalLight>) return ComponentType::DIRECTIONAL_LIGHT;
		else if constexpr (std::is_same_v<Ty, SpotLight>) return ComponentType::SPOT_LIGHT;
		else if constexpr (std::is_same_v<Ty, AmbientLight>) return ComponentType::AMBIENT_LIGHT;
		else if constexpr (std::is_same_v<Ty, Skybox>) return ComponentType::SKYBOX;
		else if constexpr (std::is_same_v<Ty, MoviePlayer>) return ComponentType::MOVIE_PLAYER;
		else if constexpr (std::is_same_v<Ty, ParticleSystem>) return ComponentType::PARTICLE_SYSTEM;
		else if constexpr (std::is_same_v<Ty, LineRenderer>) return ComponentType::LINE_RENDERER;
		else if constexpr (std::is_same_v<Ty, Animator>) return ComponentType::ANIMATOR;
		else if constexpr (std::is_same_v<Ty, AudioSource>) return ComponentType::AUDIO_SOURCE;
		else if constexpr (std::is_same_v<Ty, AudioListener>) return ComponentType::AUDIO_LISTENER;
		else if constexpr (std::is_same_v<Ty, AudioFilter>) return ComponentType::AUDIO_FILTER;
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
		uint32 tag = 0, 
		bool visible = true, 
		bool constant = false
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
		uint32 tag = 0, 
		bool visible = true, 
		bool constant = false
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
	template <DerivedComponentType Ty, class... Args> void addComponent(Args... args) {
		m_components.emplace(cast_enum_type<Ty>(), UniquePointer<Ty>::create(std::forward<Args>(args)...));
	}
	/**
	 * @brief add a component of a certain type
	 * 
	 * @tparam Ty type of component to add
	 * @tparam Args arguments to construct that component
	 */
	template <DerivedComponentType Ty> void addComponent(UniquePointer<Ty>&& component) {
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
		return mConstant;
	}

private:
	uint32 m_tag;
	bool m_visible;
	bool mConstant;

	UniquePointer<Script> m_script;
	std::unordered_map<ComponentType, UniquePointer<ComponentBase>> m_components;
};

} // namespace lyra
