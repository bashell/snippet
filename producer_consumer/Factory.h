#ifndef _FACOTRY_H_
#define _FACTORY_H_

#include <vector>
#include "Buffer.h"
#include "ProduceThread.h"
#include "ConsumeThread.h"

class Factory {
 public:
  Factory(size_t bufferSize, size_t producerNum, size_t consumerNum);
  void start();

 private:
  size_t bufferSize_;
  Buffer buffer_;
  size_t producerNum_;
  size_t consumerNum_;
  std::vector<ProduceThread> produceThreads_;
  std::vector<ConsumeThread> consumeThreads_;
};

#endif  // _FACTORY_H_
