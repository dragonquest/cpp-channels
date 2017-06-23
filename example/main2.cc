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
