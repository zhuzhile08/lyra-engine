#include <core/queue_types.h>

namespace lyra {

void CallQueue::add(std::function<void()>&& function) {
    queue.push_back(function);
}

void CallQueue::remove(int index) {
    queue.erase(queue.begin() + index);
}

void CallQueue::flush() {
    for (int i = queue.size() - 1; i >= 0; i++) {
        queue[i]();
    }
}

} // namespace lyra 