#include <iostream>
#include <string>
#include <thread>

#include "MessageQueue.hpp"

using namespace libmsgpass;

static void ThreadPrinter(std::string txtOut, MessageQueue& inQueue, MessageQueue& outQueue,
                          std::ostream& os) {
    while (1) {
        inQueue.Receive([&](int what, int arg1, int arg2, void* obj) {
            // Print the text and forward the message to the output queue
            os << txtOut;
            outQueue.Send(what, arg1, arg2, obj);
        });
    }
}

int main() {
    MessageQueue toTh1;
    MessageQueue toTh2;

    // Start both threads
    std::thread thHello(ThreadPrinter, "Hello ", std::ref(toTh1), std::ref(toTh2),
                        std::ref(std::cout));
    std::thread thWorld(ThreadPrinter, "World!\n", std::ref(toTh2), std::ref(toTh1),
                        std::ref(std::cout));

    // Send the first message to first thread to start the loop
    toTh1.Send(1, 2, 3, nullptr);

    // Should not reach here
    thHello.join();
    thWorld.join();

    return 0;
}
