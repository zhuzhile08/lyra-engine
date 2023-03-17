/*************************
 * @file   script.h
 * @author Zhile Zhu (zhuzhile08@gmail.com)
 * 
 * @brief  a script node that defines object behavoir
 * 
 * @date   2022-29-8
 * 
 * @copyright Copyright (c) 2022
*************************/

#pragma once

namespace lyra {

class GameObject;

// script component
struct Script {
	virtual void init(void) { };
	virtual void update(void) { };
	virtual void physics_update(void) { };

	GameObject* node;
};

} // namespace lyra
