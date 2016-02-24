#ifndef _THREAD_SAFE_STACK_H_
#define _THREAD_SAFE_STACK_H_

#include <exception>
#include <memory>
#include <stack>
#include <mutex>

struct empty_stack : std::exception {
  const char *what() const throw() {
  
  }
};

/*
 ** 只提供两个接口: push()和pop(). 其中对pop()进行了重载
 */
template <typename T>
class ThreadSafeStack {
 public:
  ThreadSafeStack() {}

  ThreadSafeStack(const ThreadSafeStack &other) {
    std::lock_guard<std::mutex> lock(other.m_);
    data_ = other.data_;  // 在构造函数体内执行拷贝, 确保结果正确
  }

  ThreadSafeStack &operator=(const ThreadSafeStack &) = delete;  // 禁止赋值

  void push(T new_value) {
    std::lock_guard<std::mutex> lock(m_);
    data_.push(new_value);
  }

  std::shared_ptr<T> pop() {  // 返回指向"弹出值"的指针, 避免产生异常
    std::lock_guard<std::mutex> lock(m_);
    if(data_.empty())
      throw empty_stack();  // 检查栈是否为空
    const std::shared_ptr<T> res(std::make_shared<T>(data_.top()));  // 定义指向栈顶的智能指针
    data_.pop();
    return res;
  }

  void pop(T &value) {  // 传入一个引用获取"弹出值"
    std::lock_guard<std::mutex> lock(m_);
    if(data_.empty())
      throw empty_stack();
    value = data_.top();  // 假定类型T支持赋值操作
    data_.pop();
  }

  bool empty() const {
    std::lock_guard<std::mutex> lock(m_);
    return data_.empty();
  }

 private:
  std::stack<T> data_;
  mutable std::mutex m_;  // 可变锁
};

#endif  /* _THREAD_SAFE_STACK_H_ */
