# C++ Channel & WaitGroup

An implementation of Channel & WaitGroup inspired by Go. 

__WARNING:__ Don't use it in production (see: https://github.com/dragonquest/cpp-channels/issues/1)

## Example (Channel & WaitGroup in action)

Examples can be found in the directory "./example":

```C++
#include <chrono>
#include <iostream>
#include <string>
#include <thread>

#include "../waitgroup.h"
#include "../channel.h"

int main() {
  Channel<std::string> chan;
  WaitGroup wg;
  wg.Add(2);

  std::thread first([&]() {
    for (int i = 0; i < 8; i++) {
      // Sending the message to the channel:
      chan << "from thread first: hello";
      std::this_thread::sleep_for(std::chrono::seconds(1));
      chan.Send("from thread first: world");
    }
    wg.Done();
  });

  std::thread second([&]() {
    for (int i = 0; i < 5; i++) {
      chan.Send("from thread second: hello");
      std::this_thread::sleep_for(std::chrono::seconds(1));
      chan.Send("from thread second: world");
    }
    wg.Done();
  });

  // checker waits for all threads to finish their
  // work and closes the channel.
  std::thread checker([&]() {
    wg.Wait();
    chan.Close();
  });

  while (1) {
    auto item = chan.TryRecv();
    if (!item) {
      break;
    }
    std::cout << *item << "\n";
  }

  std::cout << "All work has been done\n";

  first.join();
  second.join();
  checker.join();

  return 0;
}
```

Output:

```
$ ./main2
from thread first: hello
from thread second: hello
from thread first: world
from thread first: hello
from thread second: world
from thread second: hello
from thread first: world
from thread second: world
from thread first: hello
from thread second: hello
from thread first: world
from thread second: world
from thread first: hello
from thread second: hello
from thread second: world
from thread first: world
from thread second: hello
from thread first: hello
from thread second: world
from thread first: world
from thread first: hello
from thread first: world
from thread first: hello
from thread first: world
from thread first: hello
from thread first: world
All work has been done
```

In order to compile the examples simply go to the './example' directory and enter ```make```.

## Links

* [Go Channels](https://golang.org/ref/spec#Channel_types)
* [Go WaitGroup](https://golang.org/pkg/sync/#WaitGroup)

## Motivation

This is one of my first C++ library and I wrote it mainly to learn more about C++.
