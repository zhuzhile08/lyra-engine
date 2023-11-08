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

class ComponentBase { 
public:
	Entity* entity() { return m_entity; }
protected:
	Entity* m_entity;
};

template <class Ty> concept DerivedComponentType = std::is_base_of_v<ComponentBase, Ty>;

class Entity : public Node<Entity> {
public:
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
	
	Entity(
		std::string_view name = "Entity", 
		Entity* parent = nullptr, 
		Script* script = nullptr, 
		uint32 tag = 0, 
		bool visible = true, 
		bool constant = false
	);
	Entity(
		Entity&& parent, 
		std::string_view name = "Entity", 
		Script* script = nullptr, 
		uint32 tag = 0, 
		bool visible = true, 
		bool constant = false
	);
	
	Entity(Entity&& entityData);
	
	void update();

	template <DerivedComponentType Ty, class... Args> void addComponent(Args... args) {
		m_components.emplace(cast_enum_type<Ty>(), UniquePointer<Ty>::create(std::forward<Args>(args)...));
	}
	template <DerivedComponentType Ty> void addComponent(UniquePointer<Ty>&& component) {
		m_components.emplace(cast_enum_type<Ty>(), std::move(component));
	}

	template <DerivedComponentType Ty> NODISCARD Ty* component() {
		return static_cast<Ty*>(m_components[cast_enum_type<Ty>()]);
	}
	template <DerivedComponentType Ty> NODISCARD Ty* component() const {
		return static_cast<Ty*>(m_components.at(cast_enum_type<Ty>()));
	}

	NODISCARD constexpr uint32 tag() const noexcept {
		return m_tag;
	}
	NODISCARD constexpr bool visible() const noexcept {
		return m_visible;
	}
	NODISCARD constexpr bool constant() const noexcept {
		return m_constant;
	}

private:
	uint32 m_tag;
	bool m_visible;
	bool m_constant;

	UniquePointer<Script> m_script;
	std::unordered_map<ComponentType, UniquePointer<ComponentBase>> m_components;
};

} // namespace lyra
