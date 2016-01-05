#include <iostream>
#include <cstdlib>
#include <unistd.h>
#include "Buffer.h"
#include "ConsumeThread.h"

ConsumeThread::ConsumeThread(Buffer &buffer) : tid_(0), buffer_(buffer) {

}

void ConsumeThread::start() {
  pthread_create(&tid_, NULL, threadFunc, this);
}

void* ConsumeThread::threadFunc(void *arg) {
  ConsumeThread *pt = static_cast<ConsumeThread *>(arg);
  pt -> run();
  return NULL;
}

void ConsumeThread::run() {
  while(1) {
    int data = buffer_.consume();
    std::cout << "consume a data: " << data << std::endl;
    sleep(1);
  }
}

void ConsumeThread::join() {
  pthread_join(tid_, NULL);
}
