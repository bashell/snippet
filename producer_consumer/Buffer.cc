#include <iostream>
#include "Buffer.h"

Buffer::Buffer(size_t size) : queue_(), mutex_(), full_(mutex_), empty_(mutex_), size_(size) {

}

Buffer::~Buffer() {

}

/*
 * 这两个函数时为了供外部调用的
 * 类内部不能调用, 否则造成死锁
 */
bool Buffer::isEmpty() const {
  mutex_.lock();
  bool ret = queue_.isEmpty();
  mutex_.unlock();
  return ret;
}

bool Buffer::isFull() const {
  mutex_.lock();
  bool ret = queue_.getSize() >= size_;
  mutex_.unlock();
  return ret;
}

void Buffer::produce(int data) {
  mutex_.lock();
  while(queue_.getSize() >= size_)
    empty_.wait();
  queue_.push(data);
  std::cout << "produce size = " << queue_.getSize() << std::endl;
  full_.notify();
  mutex_.unlock();
}

/*
 * 如果中间assert失败
 * 此时mutex没有解锁, 造成析构失败
 */
int Buffer::consume() {
  mutex_.lock();
  // 这里不能调用Buffer本身的isEmpty, 否则造成死锁
  while(queue_.isEmpty())
    full_.wait();
  int ret = queue_.top();
  queue_.pop();
  std::cout << "consume size = " << queue_.getSize() << std::endl;
  empty_.notify();
  mutex_.unlock();
  return ret;
}
