#include <condition_variable>
#include <iostream>
#include <list>
#include <mutex>
#include <thread>

class MessageQueue {
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

   public:
    void Send(int what, int arg1, int arg2, void* obj) {
        mutex_.lock();
        queue_.emplace_back(what, arg1, arg2, obj);
        mutex_.unlock();
        cond_var_.notify_one();
    }

    template <typename Oper>
    void Receive(Oper oper) {
        Message msg;
        Dequeue(msg);
        oper(msg.what, msg.arg1, msg.arg2, msg.obj);
    }

   private:
    void Dequeue(Message& message) {
        std::unique_lock<std::mutex> lock_guard(mutex_);
        cond_var_.wait(lock_guard, [this]() { return !queue_.empty(); });
        message = queue_.front();
        queue_.pop_front();
    }

    std::condition_variable cond_var_;
    std::mutex mutex_;
    std::list<Message> queue_;
};

static void PrintHello(MessageQueue& inQueue, MessageQueue& outQueue, std::ostream& os) {
    while (1) {
        inQueue.Receive([&](int what, int arg1, int arg2, void* obj) {
            os << "Hello ";
            outQueue.Send(what, arg1, arg2, obj);
        });
    }
}

static void PrintWorld(MessageQueue& inQueue, MessageQueue& outQueue, std::ostream& os) {
    while (1) {
        inQueue.Receive([&](int what, int arg1, int arg2, void* obj) {
            os << "World!" << std::endl;
            outQueue.Send(what, arg1, arg2, obj);
        });
    }
}

int main(int argc, char const* argv[]) {
    MessageQueue toTh1;
    MessageQueue toTh2;
    std::thread thHello(PrintHello, std::ref(toTh1), std::ref(toTh2), std::ref(std::cout));
    std::thread thWorld(PrintWorld, std::ref(toTh2), std::ref(toTh1), std::ref(std::cout));

    toTh1.Send(1, 2, 3, nullptr);

    thHello.join();
    thWorld.join();

    return 0;
}
