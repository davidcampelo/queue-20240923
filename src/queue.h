/**
 * @file queue.h
 * @brief This file provides the declaration of a thread-safe queue class.
 */
#ifndef SRC_QUEUE_H_
#define SRC_QUEUE_H_

#include <atomic>
#include <condition_variable>  // NOLINT
#include <mutex>               // NOLINT
#include <stdexcept>
#include <type_traits>

/**
 * @brief Concept to check if a type is a primitive type.
 *
 * @tparam T The type to check.
 */
template <typename T>
concept Primitive = std::is_fundamental_v<T>;

/**
 * @brief A thread-safe queue implementation.
 *
 * @tparam T The type of the elements in the queue.
 *
 * @details This class provides a thread-safe queue implementation that can be
 * used to communicate between threads. The queue has a fixed size and will
 * overwrite the oldest element when it is full. The queue provides methods to
 * push and pop elements, as well as a method to pop an element with a timeout.
 * The queue is implemented using a circular buffer and uses a mutex and a
 * condition variable to synchronize access to the buffer.
 *
 * @note This class is not copyable or movable.
 */
template <Primitive T>
class Queue {
 public:
  /**
   * @brief Construct a new Queue object.
   *
   * @param size The size of the queue.
   */
  explicit Queue(int size) : data_{new T[size]}, size_{size}, count_{0}, is_stopping_{false} {}

  /**
   * @brief Destroy the Queue object.
   */
  virtual ~Queue() {
    is_stopping_ = true;
    cv_.notify_all();
    delete[] data_;
  }

  /**
   * @brief Deleted copy constructor.
   */
  Queue(const Queue&) = delete;

  /**
   * @brief Deleted move constructor.
   */
  Queue(Queue&&) = delete;

  /**
   * @brief Deleted copy assignment operator.
   */
  Queue& operator=(const Queue&) = delete;

  /**
   * @brief Deleted move assignment operator.
   */
  Queue& operator=(Queue&&) = delete;

  /**
   * @brief Push an element to the queue.
   *
   * @param value The value to push to the queue.
   */
  void Push(T value) {
    std::unique_lock<std::mutex> lock{mutex_};
    if (count_ == size_) {
      head_ = (head_ + 1) % size_;
      count_--;
    }
    Enqueue(value);
    cv_.notify_one();
  }

  /**
   * @brief Pop an element from the queue.
   *
   * @return T The value popped from the queue.
   */
  T Pop() {
    std::unique_lock<std::mutex> lock{mutex_};
    cv_.wait(lock, [this] { return count_ > 0 || is_stopping_; });
    if (is_stopping_ && count_ == 0) {
      return T{};
    }
    return Dequeue();
  }

  /**
   * @brief Pop an element from the queue with a timeout.
   *
   * @param milliseconds The timeout in milliseconds.
   * @return T The value popped from the queue.
   *
   * @throws std::runtime_error If the timeout is reached.
   */
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

  /**
   * @brief Get the number of elements in the queue.
   *
   * @return int The number of elements in the queue.
   */
  int Count() const { return count_; }

  /**
   * @brief Get the size of the queue.
   *
   * @return int The size of the queue.
   */
  int Size() const { return size_; }

 private:
  /**
   * @brief Enqueue an element to the queue.
   *
   * @param value The value to enqueue.
   */
  inline void Enqueue(T value) {
    data_[tail_] = value;  // NOLINT(cppcoreguidelines-pro-bounds-pointer-arithmetic)
    tail_ = (tail_ + 1) % size_;
    count_++;
  }

  /**
   * @brief Dequeue an element from the queue.
   *
   * @return T The value dequeued.
   */
  inline T Dequeue() {
    T value = data_[head_];  // NOLINT(cppcoreguidelines-pro-bounds-pointer-arithmetic)
    head_ = (head_ + 1) % size_;
    count_--;
    return value;
  }

  T* data_;                       /*!< The data buffer. */
  int size_;                      /*!< The size of the queue. */
  int head_{0};                   /*!< The head of the queue. */
  int tail_{0};                   /*!< The tail of the queue. */
  std::atomic<int> count_;        /*!< The number of elements in the queue. */
  std::atomic<bool> is_stopping_; /*!< Flag to indicate if the queue is stopping. */
  mutable std::mutex mutex_;      /*!< The mutex to synchronize access to the queue. */
  std::condition_variable cv_;    /*!< The condition variable to notify waiting threads. */
};

#endif  // SRC_QUEUE_H_
