#include <chrono>
#include "ThreadPool.h"

int main()
{
  ThreadPool tp;
  std::thread thd1([&tp] {
        for(int i = 0; i < 10; ++i) {
          auto thdId = std::this_thread::get_id();
          tp.AddTask([thdId] { std::cout << "Thread1 ID: " << thdId << std::endl; });
        }
      });
  std::thread thd2([&tp] {
        for(int i = 0; i < 10; ++i) {
          auto thdId = std::this_thread::get_id();
          tp.AddTask([thdId] { std::cout << "Thread2 ID: " << thdId << std::endl; });
        }
      });
  std::this_thread::sleep_for(std::chrono::seconds(2));
  tp.Stop();
  thd1.join();
  thd2.join();

  return 0;
}
