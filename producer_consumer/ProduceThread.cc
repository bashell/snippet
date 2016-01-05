#include <iostream>
#include <cstdlib>
#include <unistd.h>
#include <time.h>
#include "Buffer.h"
#include "ProduceThread.h"

ProduceThread::ProduceThread(Buffer &buffer) : tid_(0), buffer_(buffer) {
  ::srand(10086);
}

void ProduceThread::start() {
  pthread_create(&tid_, NULL, threadFunc, this);
}

void* ProduceThread::threadFunc(void *arg) {
  ProduceThread *pt = static_cast<ProduceThread *>(arg);
  pt -> run();
  return NULL;
}

void ProduceThread::run() {
  while(1) {
    int data = rand() % 1000;
    std::cout << "produce a data : " << data << std::endl;
    buffer_.produce(data);
    sleep(3);
  }
}

void ProduceThread::join() {
  pthread_join(tid_, NULL);
}
