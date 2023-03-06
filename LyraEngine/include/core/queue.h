/*************************
 * @file queue.h
 * @author zhuzhile08 (zhuzhile08@gmail.com)
 * 
 * @brief A function pointer queue that executes the functions from first to last added
 * 
 * @date 2022-06-09
 * 
 * @copyright Copyright (c) 2022
 *************************/

#pragma once

#include <lyra.h>

#include <deque>
#include <functional>

namespace lyra {

/**
 * @brief a queue of functions to be executed
 *
 * @tparam Ty type of functions to queue
 */
class CallQueue {
public:
	/**
	 * @brief add a function to the queue
	 *
	 * @param function the function (probably) as a lambda
	 */
	constexpr void add(std::function<void()>&& function) {
		m_queue.push_back(function);
	}
	/**
	 * @brief remove a function from the queue without calling it
	 *
	 * @param index index of the function to remove
	 */
	constexpr void remove(const uint32& index) {
		m_queue.erase(m_queue.begin() + index);
	}

	/**
	 * @brief flush the queue whilst calling all the functions
	 */
	constexpr void flush() const {
		for (uint32 i = 0; i < m_queue.size(); i++) (m_queue.at(i))();
	}

private:
	std::deque<std::function<void()>> m_queue;
};

} // namespace lyra
