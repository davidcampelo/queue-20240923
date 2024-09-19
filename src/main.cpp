#include <iostream>
#include <thread>

#include "queue.h"

void TestPushPop() {
  Queue<int> q{3};
  std::cout << "Testing Push and Pop" << std::endl;
  q.Push(1);
  q.Push(2);
  q.Push(3);
  std::cout << "Queue Count: " << q.Count() << std::endl;
  std::cout << "Popped: " << q.Pop() << std::endl;
  std::cout << "Popped: " << q.Pop() << std::endl;
  std::cout << "Popped: " << q.Pop() << std::endl;
  std::cout << "Queue Count after pops: " << q.Count() << std::endl;
}

void TestPopWithTimeout() {
  Queue<int> q{3};
  std::cout << "Testing PopWithTimeout" << std::endl;
  try {
    std::cout << "Attempting to Pop with timeout..." << std::endl;
    q.PopWithTimeout(1000);
  } catch (const std::exception& e) {
    std::cout << e.what() << std::endl;
  }
  q.Push(10);
  std::cout << "Popped after push: " << q.PopWithTimeout(1000) << std::endl;
}

void TestMultithreadedPushPop() {
  Queue<int> q{5};
  std::cout << "Testing multithreaded Push and Pop" << std::endl;
  std::thread producer{[&q]() {
    for (int i{0}; i < 10; ++i) {
      q.Push(i);
      std::cout << "Pushed: " << i << std::endl;
      std::this_thread::sleep_for(std::chrono::milliseconds{500});
    }
  }};
  std::thread consumer{[&q]() {
    for (int i{0}; i < 10; ++i) {
      int value = q.Pop();
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
