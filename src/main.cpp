#include <iostream>
#include <thread>

#include "queue.h"

void TestPushPop() {
  Queue<int> queue{3};
  std::cout << "Testing Push and Pop" << std::endl;
  queue.Push(1);
  queue.Push(2);
  queue.Push(3);
  std::cout << "Queue Count: " << queue.Count() << std::endl;
  std::cout << "Popped: " << queue.Pop() << std::endl;
  std::cout << "Popped: " << queue.Pop() << std::endl;
  std::cout << "Popped: " << queue.Pop() << std::endl;
  std::cout << "Queue Count after pops: " << queue.Count() << std::endl;
}

void TestPopWithTimeout() {
  Queue<int> queue{3};
  std::cout << "Testing PopWithTimeout" << std::endl;
  try {
    std::cout << "Attempting to Pop with timeout..." << std::endl;
    queue.PopWithTimeout(1000);
  } catch (const std::exception& e) {
    std::cout << e.what() << std::endl;
  }
  queue.Push(10);
  std::cout << "Popped after push: " << queue.PopWithTimeout(1000) << std::endl;
}

void TestMultithreadedPushPop() {
  Queue<int> queue{5};
  std::cout << "Testing multithreaded Push and Pop" << std::endl;
  std::thread producer{[&queue]() {
    for (int i{0}; i < 10; ++i) {
      queue.Push(i);
      std::cout << "Pushed: " << i << std::endl;
      std::this_thread::sleep_for(std::chrono::milliseconds{500});
    }
  }};
  std::thread consumer{[&queue]() {
    for (int i{0}; i < 10; ++i) {
      int value = queue.Pop();
      std::cout << "Popped: " << value << std::endl;
      std::this_thread::sleep_for(std::chrono::milliseconds{700});
    }
  }};
  producer.join();
  consumer.join();
}

int main() {
  TestPushPop();
  TestPopWithTimeout();
  TestMultithreadedPushPop();
  return 0;
}
