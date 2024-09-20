#include "queue.h"

#include <gtest/gtest.h>

class QueueTest : public ::testing::Test {
 protected:
  void SetUp() override {
    queue = new Queue<int>{3};  // NOLINT(cppcoreguidelines-owning-memory)
    EXPECT_EQ(queue->Size(), 3);
  }

  void TearDown() override { delete queue; }

  Queue<int>* queue;  // NOLINT(cppcoreguidelines-non-private-member-variables-in-classes)
};

TEST_F(QueueTest, PushPop) {
  queue->Push(1);
  queue->Push(2);
  queue->Push(3);
  EXPECT_EQ(queue->Count(), 3);
  EXPECT_EQ(queue->Pop(), 1);
  EXPECT_EQ(queue->Pop(), 2);
  EXPECT_EQ(queue->Pop(), 3);
  EXPECT_EQ(queue->Count(), 0);
}

TEST_F(QueueTest, PushOverwritesWhenFull) {
  queue->Push(1);
  queue->Push(2);
  queue->Push(3);
  queue->Push(4);
  EXPECT_EQ(queue->Count(), 3);
  EXPECT_EQ(queue->Pop(), 2);
  EXPECT_EQ(queue->Pop(), 3);
  EXPECT_EQ(queue->Pop(), 4);
  EXPECT_EQ(queue->Count(), 0);
}

TEST_F(QueueTest, PopFromEmptyQueueBlocks) {
  std::thread thread{[this]() {
    std::this_thread::sleep_for(std::chrono::milliseconds{100});
    queue->Push(1);
  }};
  EXPECT_EQ(queue->Pop(), 1);
  thread.join();
}

TEST_F(QueueTest, PopWithTimeoutSucceds) {
  std::thread thread{[this]() {
    std::this_thread::sleep_for(std::chrono::milliseconds{100});
    queue->Push(1);
  }};
  EXPECT_EQ(queue->PopWithTimeout(1000), 1);
  thread.join();
}

TEST_F(QueueTest, PopWithTimeoutFails) {
  std::thread thread{[this]() {
    std::this_thread::sleep_for(std::chrono::milliseconds{1000});
    queue->Push(1);
  }};
  EXPECT_THROW(queue->PopWithTimeout(100), std::runtime_error);
  thread.join();
}

TEST_F(QueueTest, MultithreadedPushPop) {
  std::thread producer{[this]() {
    for (int i{0}; i < 10; ++i) {
      queue->Push(i);
      std::this_thread::sleep_for(std::chrono::milliseconds{100});
    }
  }};
  std::thread consumer{[this]() {
    for (int i{0}; i < 10; ++i) {
      EXPECT_EQ(queue->Pop(), i);
    }
  }};
  producer.join();
  consumer.join();
}
