#pragma once

#include <vector>
#include <thread>
#include <atomic>
#include <functional>
#include <immintrin.h>

class ThreadPool
{
public:
    ThreadPool(size_t threads);
    ~ThreadPool();

    void run(std::vector<std::function<void(int)>>& tasks);

    size_t getThreadCount() const { return workers.size(); }

private:
static inline void cpuRelax(int& spins)
{
    if(spins < 250)
    {
        _mm_pause();
        spins++;
    }
    else
    {
        std::this_thread::yield();
        spins = 0;
    }
}
    void workerLoop(int threadID);

    std::vector<std::thread> workers;
    std::vector<std::function<void(int)>> tasks;
    
    std::atomic<size_t> nextTask{0};
    std::atomic<size_t> finished{0};
    std::atomic<size_t> currentRunID{0};
    std::atomic<bool> stop{false};
};