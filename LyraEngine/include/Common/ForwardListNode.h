/*************************
 * @file ForwardListNode.h
 * @author Zhile Zhu (zhuzhile08@gmail.com)
 * 
 * @brief Basic forward list node
 * 
 * @date 2024-04-10
 * 
 * @copyright Copyright (c) 2024
 *************************/

#pragma once

#include <Common/Common.h>
#include <Common/Allocator.h>

namespace lyra {

struct ForwardListNodeBase {
public:
	ForwardListNodeBase* next = nullptr;
};

template <class Ty> class ForwardListNode : public ForwardListNodeBase {
public:
	using value_type = Ty;

	constexpr ForwardListNode() = default;
	template <class... Args> constexpr ForwardListNode(Args&&... args) noexcept : value(std::forward<Args>(args)...) { }

	value_type value { }; 
};

} // namespace lyra
