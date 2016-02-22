## 生产者消费者 -- 车间类封装

---
**框架分析**
- Condition内部含有MutexLock的引用. 因为多个Condition要与同一个MutexLock进行交互
- Buffer包含了MutexLock和Condition, 是在原生的Queue上面包装了一层互斥与同步的机制, 从而实现了一个线程安全的缓冲区
- 因为多个线程要与同一个Buffer进行交互, 因此采用Buffer的引用
- Buffer内置了同步与互斥的机制, 使得生产者进程和消费者进程不必关心竞态条件, 模块之间的独立性增强, 符合"高内聚 低耦合"原则
