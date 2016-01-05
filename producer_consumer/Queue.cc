#include <iostream>
#include <cstdlib>
#include <cassert>
#include "Queue.h"

Queue::Queue() : head_(NULL), tail_(NULL), size_(0) { 

}

Queue::~Queue() {
  clear();
}

void Queue::push(int data) {
  Node *p = new Node;
  if(NULL == p) {
    std::cerr << "new failure" << std::endl;
    exit(EXIT_FAILURE);
  }
  p -> data_ = data;
  p -> next_ = NULL;
  if(isEmpty()) {
    head_ = tail_ = p;
  } else {
    tail_ -> next_ = p;
    tail_ = p;
  }
  ++size_;
}

void Queue::pop() {
  assert(!isEmpty());
  Node *p = head_;
  head_ = head_ -> next_;
  delete(p);
  p = NULL;
  --size_;
}

int Queue::top() const {
  assert(!isEmpty());
  return head_ -> data_;
}

void Queue::clear() {
  while(!isEmpty())
    pop();
}

bool Queue::isEmpty() const {
  return head_ == NULL;
}

size_t Queue::getSize() const {
  return size_;
}
