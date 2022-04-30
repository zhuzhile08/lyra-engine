#pragma once

#include <vector>
#include <functional>

namespace lyra {

/**
 * @brief a queue of functions to be executed
 * 
 * @tparam Tp type of functions to queue
 */
class CallQueue {
public:
    CallQueue();

    /**
     * @brief add a function to the queue
     * 
     * @param function the function (probably) as a lambda
     */
    void add(std::function<void()>&& function);
    /**
     * @brief remove a function from the queue without calling it
     * 
     * @param index index of the function to remove
     */
    void remove(int index);
    /**
     * @brief flush the queue whilst calling all the functions
     */
    void flush();

private:
    std::vector<std::function<void()>> queue;
};

} // namespace lyra
