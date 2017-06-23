#include <chrono>
#include <iostream>
#include <string>
#include <thread>
#include <vector>

#include "../waitgroup.h"
#include "../channel.h"

struct Result {
	int result;
	int id; // of the worker who provided the result
};

// Worker receives a number from the jobs/input channel and calculates +1,
// then it sends the calculated result to the results channel
void Worker(int id, Channel<int>* jobs, Channel<Result>* results, WaitGroup* wg) {
  for(auto job : *jobs) {
    std::this_thread::sleep_for(std::chrono::milliseconds(1));

    auto val = *job; // XXX(an): optional.value() is not impl at the time of coding :(-
    Result result{val + 1, id};
    results->Send(result);
  }
  wg->Done();
  return;
}

int main() {
  Channel<int> jobs;
  Channel<Result> results;
  WaitGroup wg;
  std::vector<std::thread> workers;

  // Spawning 20 Workers. 
  for (int i = 1; i <= 20; i++) {
    wg.Add(1);

    std::thread([&, i]() {
    	Worker(i, &jobs, &results, &wg);
    }).detach();
  }

  // Sending work to the Worker:
  for (int i = 1; i <= 5000; i++) {
    jobs.Send(i);
  }
  jobs.Close();

  // Waiting for all threads to finish its
  // work and then close the results channel.
  std::thread([&]() {
  	wg.Wait();
  	results.Close();
  }).detach();

  while (1) {
    auto result = results.TryRecv();

    if (!result) {
      break;
    }

    auto resp = result;
    std::cout << "Result: " << resp->result << " from #" << resp->id << "\n";
  }

  return 0;
}
