#include <core/queue_types.h>

namespace lyra {

CallQueue::CallQueue() { }

void CallQueue::add(std::function<void()>&& function) {
    queue.push_back(function);
}

void CallQueue::remove(int index) {
    queue.erase(queue.begin() + index);
}

void CallQueue::flush() {
    for (auto func = queue.rbegin(); func != queue.rend(); func++) {
        (*func)();
    }
}

} // namespace lyra 