#pragma once

#include <core/decl.h>
#include <deque>
#include <functional>

namespace lyra {

/**
 * @brief a queue of functions to be executed
 *
 * @tparam Tp type of functions to queue
 */
class CallQueue {
public:
	CallQueue() { }

	/**
	 * @brief add a function to the queue
	 *
	 * @param function the function (probably) as a lambda
	 */
	void add(std::function<void()>&& function) {
		m_queue.push_back(function);
	}
	/**
	 * @brief remove a function from the queue without calling it
	 *
	 * @param index index of the function to remove
	 */
	void remove(int index) {
		m_queue.erase(m_queue.begin() + index);
	}

	/**
	 * @brief flush the queue whilst calling all the functions
	 */
	void flush() const {
		for (uint32 i = 0; i < m_queue.size(); i++) (m_queue.at(i))();
	}

private:
	std::deque<std::function<void()>> m_queue;
};

} // namespace lyra
