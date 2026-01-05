/*
 * 知识点：RAII资源获取即初始化
 * 源项目：e:\3.esp-brookesia\esp-brookesia\products\speaker\main\modules\system.cpp 第112行
 * C++ Primer中文版（第五版）：第12章 动态内存 - 12.1 动态内存与智能指针
 * Effective+C++中文版（第三版）：条款13 以对象管理资源
 */
#include <iostream>
#include <mutex>
#include <thread>
#include <vector>

// 模拟LvLockGuard，使用RAII管理锁
class LvLockGuard
{
public:
    // 获取资源
    explicit LvLockGuard(std::mutex &mtx) : mutex(mtx)
    {
        std::cout << "Acquiring lock..." << std::endl;
        mutex.lock();
        locked = true;
    }

    // 禁止拷贝构造
    LvLockGuard(const LvLockGuard &) = delete;
    LvLockGuard &operator=(const LvLockGuard &) = delete;

    // 允许移动构造
    LvLockGuard(LvLockGuard &&other) noexcept : mutex(other.mutex), locked(other.locked)
    {
        other.locked = false;
    }

    LvLockGuard &operator=(LvLockGuard &&other) noexcept = delete;

    // 释放资源
    ~LvLockGuard()
    {
        if (locked)
        {
            std::cout << "Releasing lock..." << std::endl;
            mutex.unlock();
        }
    }

private:
    std::mutex &mutex;
    bool locked = false;
};

// 共享资源
class SharedResource
{
public:
    SharedResource() : value(0) {}

    void increment()
    {
        // 使用RAII锁保护临界区
        LvLockGuard guard(mutex);

        // 临界区：修改共享资源
        ++value;
        std::cout << "Thread " << std::this_thread::get_id() << ": Value incremented to " << value << std::endl;

        // 模拟一些工作
        std::this_thread::sleep_for(std::chrono::milliseconds(10));

        // 锁会在guard销毁时自动释放
    }

    int getValue() const
    {
        LvLockGuard guard(const_cast<std::mutex &>(mutex));
        return value;
    }

private:
    mutable std::mutex mutex; // mutable允许const方法修改
    int value;
};

void worker(SharedResource &resource, int iterations)
{
    for (int i = 0; i < iterations; ++i)
    {
        resource.increment();
    }
}

int main()
{
    SharedResource resource;

    std::cout << "Initial value: " << resource.getValue() << std::endl;

    // 创建多个线程访问共享资源
    std::vector<std::thread> threads;
    for (int i = 0; i < 3; ++i)
    {
        threads.emplace_back(worker, std::ref(resource), 5);
    }

    // 等待所有线程完成
    for (auto &t : threads)
    {
        t.join();
    }

    std::cout << "Final value: " << resource.getValue() << std::endl;

    return 0;
}
