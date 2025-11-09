# 基本原理

`pthread_cond_t` 是 POSIX 线程库中用于线程间同步的重要机制，通常与 `pthread_mutex_t` 配合使用，实现“等待/通知”模式。
用于解决“忙等待”（busy-waiting）带来的 CPU 资源浪费问题。

条件变量 (Condition Variable): `pthread_cond_t` 本身是一个同步对象，它不存储任何状态信息。它只是一个“等待队列”，线程可以在它上面等待，或者通知它唤醒等待的线程。

互斥锁 (Mutex): `pthread_mutex_t` 用于保护共享数据（临界区），确保同一时间只有一个线程可以访问。在使用条件变量时，互斥锁还用于保护条件变量本身，确保“检查条件-等待”或“修改条件-通知”这两个操作是原子的。

等待 (pthread_cond_wait):

当一个线程需要等待某个条件（例如，队列为空）变为真时，它会调用 pthread_cond_wait。
关键步骤：
1. 首先，线程必须已经获取了与条件变量关联的互斥锁。
2. pthread_cond_wait 函数会自动释放该互斥锁。
3. 线程进入睡眠状态，加入到条件变量的等待队列中。
4. 当其他线程通过 pthread_cond_signal 或 pthread_cond_broadcast 唤醒它时，pthread_cond_wait 会自动重新获取互斥锁，然后返回。

通知 (pthread_cond_signal / pthread_cond_broadcast):

- 当一个线程改变了共享数据的状态（例如，向队列放入了数据），使得等待的线程可以继续执行时，它会调用 `pthread_cond_signal`。
- `pthread_cond_signal`: 唤醒至少一个正在该条件变量上等待的线程。如果只有一个线程在等待，那么该线程被唤醒。
- `pthread_cond_broadcast`: 唤醒所有正在该条件变量上等待的线程。当条件改变可能影响所有等待者时使用（例如，关闭一个生产者-消费者队列）。
关键点： 发送通知的线程通常在持有互斥锁的状态下调用通知函数，然后才释放锁。

重要实践原则

- 始终在互斥锁保护下操作: 检查条件、修改条件、调用 wait、signal/broadcast 都应在互斥锁的保护下进行。
- 使用循环检查条件: 由于线程被唤醒后重新获取锁需要时间，在这段时间内，其他线程可能已经处理了该条件。因此，wait 返回后，必须再次检查条件是否真的满足。如果条件不满足，应继续 wait。这称为“虚假唤醒”（Spurious Wakeup）的处理。

