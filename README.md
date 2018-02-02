
# MsgPassing

This module contains the implementation of a simple message queue library and some applications to test its functionality.

## MessageQueue

The MessageQueue class provides 5 simple methods for interacting with the queue.

### Send

Posts a new message to the queue. The user must provide a 'what' identifying the message type, two arguments 'arg1' and 'arg2' and a void ponter to an object. The object lifetime is not handled by the message queue itself, so it is the user responsibility to guarantee that this pointer will be valid when the message is processed.

Example:
```cpp
MessageQueue msgQueue;
msgQueue.Send(arg, value1, value2, &object);
```

Since the underlying storage for the message queue is an std::list, the time complexity of this method is constant, O(1).

### Receive

Waits for a message to be available in the queue and executes a callable object provided by the user, removing the message from the queue in the process.

```cpp
msgQueue.Receive([&](int what, int arg1, int arg2, void* obj) {
    if(what == 1) {
        MyObject* myobj = static_cast<MyObject*>(obj);
        myobj->Func(arg1, arg2);
    }
});
```


Although the wait time can depend on the message availability, the time complexity to manipulate the queue storage is constant, O(1).

### Count

Retrieves the number of elements in the queue.

```cpp
MessageQueue msgQueue;
std::cout << "Number of messages in the queue: " << msgQueue.Count() << "\n";
```
Time complexity is O(1).

**Note**: Since the c++11 standard the method size() in std::list is required to have constant time complexity. In the previous versions this was suggested but not required. Some pre c++11 STL implementations have this method implemented with linear O(n) complexity.

##### ClearMsgType

Clears all the messages in the queue that match a specific 'what' provided by the user.

```cpp
MessageQueue msgQueue;
// All the messages in the queue with the 'what' field equal to 1 will be removed
msgQueue.ClearMsgType(1);
```
Time complexity is O(n).

### Peek

Receives a callable object provided by the user and calls this object if there is a message waiting in the queue, returning **true** if there was a message in the queue and **false** if the queue was empty. The message is not removed from the queue.

```cpp
bool result = msgQueue.Peek([&](int what, int arg1, int arg2, void*) {
    std::cout << "Next element in the queue is: "
        "what(" << what << ") "
        "arg1(" << arg1 << ") "
        "arg2(" << arg2 << ") " << std::endl;
});
if(!result) {
	std::cout << "The queue is empty" << std::endl;
}
```

Time complexity is O(1).

***

## HelloWorld

This application spawns two threads, one for printing "Hello " and one for printing "World!". The synchronization mechanism used between the threads is a message queue. 

Once the two threads are spawned, each one with a message queue pointing to the other, a first message is posted to the first thread by the main application. Then, the first thread prints its message and forwards this message to the other thread which also prints its message and send the message back, making an infinite loop.

***

## TestMsgQueue

This application uses the [catch](https://github.com/catchorg/Catch2) test framework to implement tests to validate the message queue functionality.

Among other tests, there is a producer-consumer test that spawns multiple threads that keep posting operations to the message queue and an equal number of threads that receive from this message queue and perform the operations. This test validates that the sum of all the operation results is consistent.

