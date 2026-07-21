#pragma once

#include <vector>
#include <thread>
#include <atomic>
#include <memory>
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
        if (spins < 1000)
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
    struct alignas(64) WorkerQueue
    {
        size_t begin;
        size_t end;
        alignas(64)
            std::atomic<size_t> current;
    };
    std::vector<std::unique_ptr<WorkerQueue>> queues;

    alignas(64) std::atomic<size_t> finished;
    alignas(64) std::atomic<size_t> currentRunID;
    std::atomic<bool> stop{ false };

    const std::vector<std::function<void(int)>>* tasks = nullptr;
};