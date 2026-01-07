#include <iostream>
#include <thread>
#include <atomic>

// int g_mycout = 0;
// 原子整型类型变量，可以像使用整型变量一样使用
std::atomic<int> g_mycout{0};

void mythread()
{
    for (int i = 0; i < 100000000; i++) {
        g_mycout++;
    }
    return;
}

int main()
{
    std::cout << "main" << " threadid = " << std::this_thread::get_id() << std::endl;
    std::thread mytobj1(mythread);
    std::thread mytobj2(mythread);
    mytobj1.join();
    mytobj2.join();
    std::cout << "两个线程都执行完毕，最终的 g_mycout 的结果是：" << g_mycout << std::endl;
    std::cout << "main 主函数执行结束!" << std::endl;
    return 0;
}
