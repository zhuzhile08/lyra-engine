/*************************
 * @file Manager.h
 * @author Zhile Zhu (zhuzhile08@gmail.com)
 * 
 * @brief ECS System and Entity managing class
 * 
 * @date 2024-01-31
 * 
 * @copyright Copyright (c) 2024
 *************************/

#pragma once

#include <Common/Common.h>
#include <Common/Vector.h>
#include <Common/Hash.h>
#include <Common/UnorderedSparseMap.h>

namespace lyra {

namespace ecs {

namespace detail {

template <class Ty> class BasicManager {
protected:
	using value_type = Ty;
	using pointer = value_type*;

	CUSTOM_HASHER(Hasher, pointer, size_type, static_cast<size_type>, ->id())
	CUSTOM_EQUAL(Equal, pointer, size_type, ->id())

public:
	BasicManager(World* world) noexcept : m_world(world) { }

	object_id uniqueId() {
		if (m_unused.empty())
			return m_lookup.size();
		else {
			auto id = m_unused.back();
			m_unused.popBack();
			return id;
		}
	}

	virtual void insert(pointer object, Archetype* archetype) = 0;
	virtual void erase(object_id id) = 0;

protected:
	UnorderedSparseMap<pointer, Archetype*, Hasher, Equal> m_lookup;
	Vector<object_id> m_unused;

	World* m_world;
};

} // namespace detail

class EntityManager : public detail::BasicManager<Entity> {
public:
	using basic_manager = detail::BasicManager<Entity>;

	EntityManager(World* world) noexcept : basic_manager(world) { }

	void insert(basic_manager::pointer entity, Archetype* archetype) override;
	void erase(object_id id) override;

	void clear(object_id id);

	Archetype*& archetype(object_id entityId);
	const Archetype* const& archetype(object_id entityId) const;
};

class SystemManager : public detail::BasicManager<BasicSystem> {
public:
	using basic_manager = detail::BasicManager<BasicSystem>;

	SystemManager(World* world) noexcept : detail::BasicManager<BasicSystem>(world) { }

	void insert(basic_manager::pointer object, Archetype* archetype) override;
	void erase(object_id id) override;
};

} // namespace ecs

} // namespace lyra
