#ifndef _SYNC_QUEUE_H_
#define _SYNC_QUEUE_H_

#include <iostream>
#include <thread>
#include <mutex>
#include <list>
#include <condition_variable>

template<typename T>
class SyncQueue {
 public:
  SyncQueue(int max_size) : m_maxSize(max_size), m_needStop(false) {}
  SyncQueue(const SyncQueue &) = delete;
  SyncQueue &operator=(const SyncQueue &) = delete;
  ~SyncQueue() {}
  
  /* 添加任务 */
  void Put(const T &x) {  // 左值拷贝
    Add(x);
  }
  void Put(T &&x) {  // 右值移动
    Add(std::forward<T>(x));  // 完美转发, 保持参数类型不变
  }

  /* 取出所有待处理任务 */
  void Take(std::list<T> &list) {
    std::unique_lock<std::mutex> locker(m_mutex);

    // 当条件满足时(Lambda返回true), wait直接返回; 当条件不满足时(Lambda返回false), wait()会解锁互斥量, 并将线程置于阻塞或等待状态;
    // 当准备数据的线程notify_one()通知条件变量时, 处理数据的线程会从睡眠状态中苏醒, 重新获取互斥锁, 并且对条件再次检查, 在条件满足的情况下, 从wait()返回并持有锁; 在条件不满足时, 线程将对互斥量解锁, 并且重新开始等待
    m_notEmpty.wait(locker, [this]{ return m_needStop || NotEmpty(); });
    if(m_needStop)
      return;
    list = std::move(m_queue);  // "窃取"队列中的所有数据
    m_notFull.notify_one();  // broadcast
  }

  /* 取出一个待处理任务 */
  void Take(T &t) {
    std::unique_lock<std::mutex> locker(m_mutex);
    m_notEmpty.wait(locker, [this]{ return m_needStop || NotEmpty(); });
    if(m_needStop)
      return;
    t = m_queue.front();
    m_queue.pop_front();
    m_notFull.notify_one();
  }

  /* 停止添加/取出任务 */
  void Stop() {
    {
      std::lock_guard<std::mutex> locker(m_mutex);
      m_needStop = true;
    }
    m_notFull.notify_all();  // notify_all不用锁保护时性能更好
    m_notEmpty.notify_all();
  }

  bool Empty() {
    std::lock_guard<std::mutex> locker(m_mutex);
    return m_queue.empty();
  }

  bool Full() {
    std::lock_guard<std::mutex> locker(m_mutex);
    return m_queue.size() == m_maxSize;
  }

  size_t Size() {
    std::lock_guard<std::mutex> locker(m_mutex);
    return m_queue.size();
  }

 private:
  template<typename F>
  void Add(F &&x) {
    std::unique_lock<std::mutex> locker(m_mutex);
    m_notFull.wait(locker, [this]{ return m_needStop || NotFull(); });
    if(m_needStop)
      return;
    m_queue.push_back(std::forward<F>(x));
    m_notEmpty.notify_one();
  }
 
  bool NotFull() const {
    bool full = m_queue.size() >= m_maxSize;
    if(full)
      std::cout << "The Queue is full, please wait..." << std::endl;
    return !full;
  }

  bool NotEmpty() const {
    bool empty = m_queue.empty();
    if(empty)
      std::cout << "The Queue is empty, please wait..." << std::endl;
    return !empty;
  }

 private:
  std::list<T> m_queue;  // 同步队列
  std::mutex m_mutex;    // 互斥锁
  std::condition_variable m_notEmpty;  // 队列不空的条件变量
  std::condition_variable m_notFull;   // 队列不满的条件变量
  int m_maxSize;    // 同步队列最大长度
  bool m_needStop;  // 停止标识
};

#endif  /* _SYNC_QUEUE_H_ */
