#ifndef _CONDITION_H_
#define _CONDITION_H_

#include <pthread.h>

// Condition中仅包含MutexLock的引用, 不需要头文件
class MutexLock;

class Condition {
 public:
  Condition(MutexLock &lock);
  ~Condition();
  void wait();
  void notify();
  void notifyAll();

 private:
  pthread_cond_t cond_;
  MutexLock &lock_;
};

#endif  // _CONDITION_H_
