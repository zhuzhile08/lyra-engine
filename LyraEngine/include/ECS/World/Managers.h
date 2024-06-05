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
#include <LSD/Vector.h>
#include <LSD/Hash.h>
#include <LSD/UnorderedSparseMap.h>

namespace lyra {

namespace ecs {

namespace detail {

template <class Ty, class Mapped> class BasicManager {
protected:
	using value_type = Ty;
	using pointer = value_type*;
	using mapped_type = Mapped;

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

	virtual void insert(pointer) = 0;
	virtual void erase(object_id) = 0;

protected:
	lsd::UnorderedSparseMap<pointer, mapped_type, Hasher, Equal> m_lookup;
	lsd::Vector<object_id> m_unused;

	World* m_world;
};

} // namespace detail

class EntityManager : public detail::BasicManager<Entity, Archetype*> {
public:
	using basic_manager = detail::BasicManager<Entity, Archetype*>;
	using pointer = basic_manager::pointer;

	EntityManager(World* world) noexcept : basic_manager(world) { }

	void insert(pointer entity) override;
	void erase(object_id id) override;

	void clear(object_id id);

	Archetype*& archetype(object_id entityId);
	Archetype* const& archetype(object_id entityId) const;
};

class SystemManager : public detail::BasicManager<BasicSystem, size_type> {
public:
	using basic_manager = detail::BasicManager<BasicSystem, size_type>;
	using pointer = basic_manager::pointer;

	SystemManager(World* world) noexcept : basic_manager(world) { }

	void insert(pointer object) override;
	void erase(object_id id) override;

	size_type hash(object_id entityId) const;

	void update();
};

} // namespace ecs

} // namespace lyra
