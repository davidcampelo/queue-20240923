#include "queue.h"

#include <gtest/gtest.h>

class QueueTest : public ::testing::Test {
 protected:
  void SetUp() override {
    q = new Queue<int>{3};
    EXPECT_EQ(q->Size(), 3);
  }

  void TearDown() override { delete q; }

  Queue<int>* q;
};

TEST_F(QueueTest, PushPop) {
  q->Push(1);
  q->Push(2);
  q->Push(3);
  EXPECT_EQ(q->Count(), 3);
  EXPECT_EQ(q->Pop(), 1);
  EXPECT_EQ(q->Pop(), 2);
  EXPECT_EQ(q->Pop(), 3);
  EXPECT_EQ(q->Count(), 0);
}

TEST_F(QueueTest, PushOverwritesWhenFull) {
  q->Push(1);
  q->Push(2);
  q->Push(3);
  q->Push(4);
  EXPECT_EQ(q->Count(), 3);
  EXPECT_EQ(q->Pop(), 2);
  EXPECT_EQ(q->Pop(), 3);
  EXPECT_EQ(q->Pop(), 4);
  EXPECT_EQ(q->Count(), 0);
}

TEST_F(QueueTest, PopFromEmptyQueueBlocks) {
  std::thread t{[this]() {
    std::this_thread::sleep_for(std::chrono::milliseconds{100});
    q->Push(1);
  }};
  EXPECT_EQ(q->Pop(), 1);
  t.join();
}

TEST_F(QueueTest, PopWithTimeoutSucceds) {
  std::thread t{[this]() {
    std::this_thread::sleep_for(std::chrono::milliseconds{100});
    q->Push(1);
  }};
  EXPECT_EQ(q->PopWithTimeout(1000), 1);
  t.join();
}

TEST_F(QueueTest, PopWithTimeoutFails) {
  std::thread t{[this]() {
    std::this_thread::sleep_for(std::chrono::milliseconds{1000});
    q->Push(1);
  }};
  EXPECT_THROW(q->PopWithTimeout(100), std::runtime_error);
  t.join();
}

TEST_F(QueueTest, MultithreadedPushPop) {
  std::thread producer{[this]() {
    for (int i{0}; i < 10; ++i) {
      q->Push(i);
      std::this_thread::sleep_for(std::chrono::milliseconds{100});
    }
  }};
  std::thread consumer{[this]() {
    for (int i{0}; i < 10; ++i) {
      EXPECT_EQ(q->Pop(), i);
    }
  }};
  producer.join();
  consumer.join();
}
