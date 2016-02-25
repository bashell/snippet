#ifndef _THREAD_POOL_H_
#define _THREAD_POOL_H_

#include <thread>
#include <list>
#include <memory>
#include <functional>
#include <atomic>
#include "SyncQueue.h"

const int MaxTaskCount = 100;

class ThreadPool {
 public:
  using Task = std::function<void()>;  // 任务类型: void()
  ThreadPool(int numThreads = std::thread::hardware_concurrency());  // default constructor
  ThreadPool(const ThreadPool &) = delete;
  ThreadPool &operator=(const ThreadPool &) = delete;
  ~ThreadPool();

  void Stop();
  void AddTask(const Task &task);
  void AddTask(Task &&task);

 private:
  void Start(int numThreads);
  void StopThreadGroup();
  void TakeAll();
  void TakeOne();

 private:
  std::list<std::shared_ptr<std::thread>> m_threadGroup;  // 线程池
  SyncQueue<Task> m_queue;     // 同步队列
  std::atomic_bool m_running;  // 原子bool, 标记是否停止
  std::once_flag m_flag;       // call_once

};

ThreadPool::ThreadPool(int numThreads /* = std::thread::hardware_concurrency() */)
    : m_queue(MaxTaskCount)
{
  Start(numThreads);
}

ThreadPool::~ThreadPool() {
  Stop();
}

void ThreadPool::Stop() {
  std::call_once(m_flag, [this]{ StopThreadGroup(); });  // 保证多线程情况下只调用一次StopThreadGroup
}

void ThreadPool::StopThreadGroup() {
  m_queue.Stop();
  m_running = false;
  for(auto t : m_threadGroup) {
    if(t)
      t -> join();
  }
  m_threadGroup.clear();  // 清空线程池
}

void ThreadPool::AddTask(const Task &task) {
  m_queue.Put(task);
}

void ThreadPool::AddTask(Task &&task) {
  m_queue.Put(std::forward<Task>(task));
}

void ThreadPool::Start(int numThreads) {
  m_running = true;
  for(int i = 0; i < numThreads; ++i) {
    m_threadGroup.push_back(std::make_shared<std::thread>(&ThreadPool::TakeAll, this));
  }
}

void ThreadPool::TakeAll() {
  while(m_running) {
    std::list<Task> list;
    m_queue.Take(list);  // 一次取出同步队列中的所有待处理任务
    for(auto &task : list) {
      if(!m_running)
        return;
      task();  // 执行任务
    }
  }
}

#endif  /* _THREAD_POOL_H_ */
