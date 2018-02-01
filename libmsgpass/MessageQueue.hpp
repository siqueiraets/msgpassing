#ifndef MESSAGEQUEUE_HPP
#define MESSAGEQUEUE_HPP

#include <condition_variable>
#include <list>
#include <mutex>

namespace libmsgpass {

class MessageQueue {
   private:
   public:
    void Send(int what, int arg1, int arg2, void* obj);
    void ClearMsgType(int what);
    size_t Count() const;

    template <typename Oper>
    void Receive(Oper oper) {
        Message msg;
        Dequeue(msg);
        oper(msg.what, msg.arg1, msg.arg2, msg.obj);
    }

    template <typename Oper>
    bool Peek(Oper oper) const {
        Message msg;
        bool result = false;

        mutex_.lock();
        if (!queue_.empty()) {
            result = true;
            msg = queue_.front();
        }
        mutex_.unlock();

        if (result) {
            oper(msg.what, msg.arg1, msg.arg2, msg.obj);
        }

        return result;
    }

   private:
    struct Message {
        int what;
        int arg1;
        int arg2;
        void* obj;

        Message(int what, int arg1, int arg2, void* obj)
            : what(what), arg1(arg1), arg2(arg2), obj(obj) {}
        Message() : what(0), arg1(0), arg2(0), obj(nullptr) {}
    };

    void Dequeue(Message& message);

    std::list<Message> queue_;
    std::condition_variable cond_var_;
    mutable std::mutex mutex_;
};

}  // namespace libmsgqueue

#endif /* MESSAGEQUEUE_HPP */
