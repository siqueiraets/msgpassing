#include "MessageQueue.hpp"

using namespace libmsgpass;

void MessageQueue::Send(int what, int arg1, int arg2, void* obj) {
    mutex_.lock();
    queue_.emplace_back(what, arg1, arg2, obj);
    mutex_.unlock();
    cond_var_.notify_one();
}

void MessageQueue::ClearMsgType(int what) {
    std::unique_lock<std::mutex> lock_guard(mutex_);
    auto it = std::begin(queue_);
    while (it != std::end(queue_)) {
        if (it->what == what) {
            queue_.erase(it++);
        } else {
            ++it;
        }
    }
}

size_t MessageQueue::Count() const {
    std::unique_lock<std::mutex> lock_guard(mutex_);
    return queue_.size();
}

void MessageQueue::Dequeue(Message& message) {
    std::unique_lock<std::mutex> lock_guard(mutex_);
    cond_var_.wait(lock_guard, [this]() { return !queue_.empty(); });
    message = queue_.front();
    queue_.pop_front();
}

