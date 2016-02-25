#ifndef _THREAD_SAFE_QUEUE_H_
#define _THREAD_SAFE_QUEUE_H_

#include <queue>
#include <memory>
#include <mutex>
#include <condition_variable>

template<typename T>
class ThreadSafeQueue {
 public:
  ThreadSafeQueue() {}
  ThreadSafeQueue(const ThreadSafeQueue &other) {
    std::lock_guard<std::mutex> lck(other.mutex);
    data_queue = other.data_queue;
  }

  void push(T new_value) {
    std::lock_guard<std::mutex> lck(mutex);
    data_queue.push(new_value);
    data_cond.notify_one();
  }

  void wait_and_pop(T &value) {  // 阻塞
    std::unique_lock<std::mutex> lck(mutex);
    data_cond.wait(lck, [this]{ return !data_queue.empty(); });  // 队列不空时wait返回
    value = data_queue.front();
    data_queue.pop();
  }
  std::shared_ptr<T> wait_pop() {
    std::unique_lock<std::mutex> lck(mutex);
    data_cond.wait(lck, [this]{ return !data_queue.empty(); });
    std::shared_ptr<T> res(std::make_shared<T>(data_queue.front()));
    data_queue.pop();
    return res;
  }

  bool try_pop(T &value) {  // 非阻塞
    std::lock_guard<std::mutex> lck(mutex);
    if(data_queue.empty())
      return false;
    value = data_queue.front();
    data_queue.pop();
    return true;
  }
  std::shared_ptr<T> try_pop() {
    std::lock_guard<std::mutex> lck(mutex);
    if(data_queue.empty())
      return std::shared_ptr<T>();
    std::shared_ptr<T> res(std::make_shared<T>(data_queue.front()));
    data_queue.pop();
    return res;
  }

  bool empty() const {
    std::lock_guard<std::mutex> lck(mutex);  // lck是mutable的
    return data_queue.empty();
  }

 private:
  mutable std::mutex mutex;  // 可变互斥锁
  std::queue<T> data_queue;  
  std::condition_variable data_cond;  // 条件变量
};

#endif  /* _THREAD_SAFE_QUEUE_H_ */
