#ifndef SRC_QUEUE_H_
#define SRC_QUEUE_H_

#include <atomic>
#include <condition_variable>
#include <mutex>
#include <stdexcept>
#include <type_traits>

template <typename T>
concept Primitive = std::is_fundamental_v<T>;

template <Primitive T>
class Queue {
 public:
  Queue(int size) : size_{size}, head_{0}, tail_{0}, count_{0}, is_stopping_{false} {
    data_ = new T[size];
  }

  virtual ~Queue() {
    is_stopping_ = true;
    cv_.notify_all();
    delete[] data_;
  }

  void Push(T value) {
    std::unique_lock<std::mutex> lock{mutex_};
    if (count_ == size_) {
      head_ = (head_ + 1) % size_;
      count_--;
    }
    Enqueue(value);
    cv_.notify_one();
  }

  T Pop() {
    std::unique_lock<std::mutex> lock{mutex_};
    cv_.wait(lock, [this] { return count_ > 0 || is_stopping_; });
    if (is_stopping_ && count_ == 0) {
      return T{};
    }
    return Dequeue();
  }

  T PopWithTimeout(int milliseconds) {
    std::unique_lock<std::mutex> lock{mutex_};
    if (!cv_.wait_for(lock, std::chrono::milliseconds(milliseconds),
                      [this] { return count_ > 0 || is_stopping_; })) {
      throw std::runtime_error{"Timeout waiting for data"};
    }
    if (is_stopping_ && count_ == 0) {
      return T{};
    }
    return Dequeue();
  }

  int Count() const { return count_; }

  int Size() const { return size_; }

 private:
  inline void Enqueue(T value) {
    data_[tail_] = value;
    tail_ = (tail_ + 1) % size_;
    count_++;
  }

  inline T Dequeue() {
    T value = data_[head_];
    head_ = (head_ + 1) % size_;
    count_--;
    return value;
  }

  T* data_;
  const int size_;
  int head_;
  int tail_;
  std::atomic<int> count_;
  std::atomic<bool> is_stopping_;
  mutable std::mutex mutex_;
  std::condition_variable cv_;
};

#endif  // SRC_QUEUE_H_
