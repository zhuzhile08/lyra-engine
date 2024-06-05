/*************************
 * @file Component.h
 * @author Zhile Zhu (zhuzhile08@gmail.com)
 * 
 * @brief Component and component manager implementation
 * 
 * @date 2024-02-11
 * 
 * @copyright Copyright (c) 2024
 *************************/

#pragma once

#include <Common/Common.h>
#include <LSD/UniquePointer.h>

namespace lyra {

class BasicComponent {
public:
	BasicComponent() noexcept = default;
	virtual ~BasicComponent() noexcept = default;

	virtual void init(void) { }
	virtual void update(void) { }

protected:
	Entity* entity;

	friend class Entity;
};

using BasicScript = BasicComponent;

} // namespace lyra
