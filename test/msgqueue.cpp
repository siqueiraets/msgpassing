#define CATCH_CONFIG_MAIN
#include "catch.hpp"

#include <functional>
#include "MessageQueue.hpp"

using namespace libmsgpass;
using std::placeholders::_1;
using std::placeholders::_2;
using std::placeholders::_3;
using std::placeholders::_4;

TEST_CASE("Message queue can send and receive", "[msgqueue]") {
    MessageQueue msgQueue;

    SECTION("The message queue should be empty when it's initialized") {
        REQUIRE(msgQueue.Count() == 0);
        REQUIRE_FALSE(msgQueue.Peek([](int, int, int, void*) {}));
    }

    SECTION("A message can be sent to the message queue") {
        msgQueue.Send(1, 2, 3, &msgQueue);

        SECTION("It's possible to peak messages in the queue") {
            REQUIRE(msgQueue.Peek([&](int what, int arg1, int arg2, void* obj) {
                REQUIRE(what == 1);
                REQUIRE(arg1 == 2);
                REQUIRE(arg2 == 3);
                REQUIRE(obj == &msgQueue);
            }));
        }

        SECTION("A message can be received from the queue") {
            msgQueue.Receive([&](int what, int arg1, int arg2, void* obj) {
                REQUIRE(what == 1);
                REQUIRE(arg1 == 2);
                REQUIRE(arg2 == 3);
                REQUIRE(obj == &msgQueue);
            });

            // Then the queue will be empty
            REQUIRE(msgQueue.Count() == 0);
            REQUIRE_FALSE(msgQueue.Peek([](int, int, int, void*) {}));
        }
    }
}

TEST_CASE("Message queue can send and receive multiple messages", "[msgqueue]") {
    MessageQueue msgQueue;

    const int msgCount = 100;
    int values[msgCount];

    // Initialize values that will be used as pointers
    for (int i = 0; i < msgCount; i++) {
        values[i] = 1000 - i;
    }

    // First send all the messages with different parameters
    for (int i = 0; i < msgCount; ++i) {
        msgQueue.Send(i, i * 2, i * 3, &values[i]);
    }

    // Then receive each message and check its contents
    for (int i = 0; i < msgCount; ++i) {
        msgQueue.Receive([i](int what, int arg1, int arg2, void* obj) {
            REQUIRE(what == i);
            REQUIRE(arg1 == i * 2);
            REQUIRE(arg2 == i * 3);

            int* valuePtr = static_cast<int*>(obj);
            REQUIRE(*valuePtr == 1000 - i);
        });
    }

    // Then the queue will be empty
    REQUIRE(msgQueue.Count() == 0);
}

TEST_CASE("Messages with a specific 'what' can be removed from the queue", "[msgqueue]") {
    MessageQueue msgQueue;

    msgQueue.Send(1, 1, 1, nullptr);
    msgQueue.Send(1, 2, 2, nullptr);
    msgQueue.Send(1, 3, 3, nullptr);
    msgQueue.Send(2, 4, 4, nullptr);
    msgQueue.Send(2, 5, 5, nullptr);
    msgQueue.Send(2, 6, 6, nullptr);
    msgQueue.Send(3, 7, 7, nullptr);
    msgQueue.Send(3, 8, 8, nullptr);
    msgQueue.Send(3, 9, 9, nullptr);

    auto checker = [](int what, int arg1, int arg2, void* obj, int whatCheck, int argCheck) {
        REQUIRE(what == whatCheck);
        REQUIRE(arg1 == argCheck);
        REQUIRE(arg2 == argCheck);
        REQUIRE(obj == nullptr);
    };
    SECTION("Messages can be deleted from the beginning") {
        msgQueue.ClearMsgType(1);
        REQUIRE(msgQueue.Count() == 6);
        msgQueue.Receive(std::bind(checker, _1, _2, _3, _4, 2, 4));
        msgQueue.Receive(std::bind(checker, _1, _2, _3, _4, 2, 5));
        msgQueue.Receive(std::bind(checker, _1, _2, _3, _4, 2, 6));
        msgQueue.Receive(std::bind(checker, _1, _2, _3, _4, 3, 7));
        msgQueue.Receive(std::bind(checker, _1, _2, _3, _4, 3, 8));
        msgQueue.Receive(std::bind(checker, _1, _2, _3, _4, 3, 9));
        REQUIRE(msgQueue.Count() == 0);
    }

    SECTION("Messages can be deleted from the middle") {
        msgQueue.ClearMsgType(2);
        REQUIRE(msgQueue.Count() == 6);
        msgQueue.Receive(std::bind(checker, _1, _2, _3, _4, 1, 1));
        msgQueue.Receive(std::bind(checker, _1, _2, _3, _4, 1, 2));
        msgQueue.Receive(std::bind(checker, _1, _2, _3, _4, 1, 3));
        msgQueue.Receive(std::bind(checker, _1, _2, _3, _4, 3, 7));
        msgQueue.Receive(std::bind(checker, _1, _2, _3, _4, 3, 8));
        msgQueue.Receive(std::bind(checker, _1, _2, _3, _4, 3, 9));
        REQUIRE(msgQueue.Count() == 0);
    }

    SECTION("Messages can be deleted from the ending") {
        msgQueue.ClearMsgType(3);
        REQUIRE(msgQueue.Count() == 6);
        msgQueue.Receive(std::bind(checker, _1, _2, _3, _4, 1, 1));
        msgQueue.Receive(std::bind(checker, _1, _2, _3, _4, 1, 2));
        msgQueue.Receive(std::bind(checker, _1, _2, _3, _4, 1, 3));
        msgQueue.Receive(std::bind(checker, _1, _2, _3, _4, 2, 4));
        msgQueue.Receive(std::bind(checker, _1, _2, _3, _4, 2, 5));
        msgQueue.Receive(std::bind(checker, _1, _2, _3, _4, 2, 6));
        REQUIRE(msgQueue.Count() == 0);
    }
}

TEST_CASE("Multiple thread can communicate using the message queue", "[msgqueue]") {

}
