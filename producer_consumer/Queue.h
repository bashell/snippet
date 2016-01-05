#ifndef _QUEUE_H_
#define _QUEUE_H_

#include <cstddef>

class Node {
  friend class Queue;
 private:
  int data_;
  Node *next_;
};

class Queue {
 public:
  Queue();
  ~Queue();
  void push(int data);
  void pop();
  int top() const;
  void clear();
  bool isEmpty() const;
  size_t getSize() const;

 private:
  Node *head_;
  Node *tail_;
  size_t size_;
};

#endif  // _QUEUE_H_
