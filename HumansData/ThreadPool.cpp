#include "ThreadPool.hpp"
#include <iostream>

ThreadPool::ThreadPool(size_t count)
{
    std::cout << "Creating " << count << " spinning threads (No OS sleep overhead)" << std::endl;
    workers.reserve(count);
    for (size_t i = 0; i < count; i++)
    {
        workers.emplace_back(
            [this, i]()
            {
                workerLoop(static_cast<int>(i));
            }
        );
    }
}

ThreadPool::~ThreadPool()
{
    stop.store(true, std::memory_order_relaxed);
    currentRunID.fetch_add(1, std::memory_order_release);

    for (auto& t : workers)
    {
        if (t.joinable())
            t.join();
    }
}

void ThreadPool::run(std::vector<std::function<void(int)>>& newTasks)
{
    tasks = std::move(newTasks);
    nextTask.store(0, std::memory_order_relaxed);
    finished.store(0, std::memory_order_relaxed);

    currentRunID.fetch_add(1, std::memory_order_release);

    size_t totalWorkers = workers.size();
    int spins = 0;

    while (finished.load(std::memory_order_relaxed) < totalWorkers)
    {
        cpuRelax(spins);
    }
}

void ThreadPool::workerLoop(int threadID)
{
    size_t localRunID = 0;

    while (!stop.load(std::memory_order_relaxed))
    {
        int spins = 0;

        while (localRunID == currentRunID.load(std::memory_order_relaxed))
        {
            cpuRelax(spins);
        }

        if (stop.load(std::memory_order_relaxed))
            break;

        localRunID = currentRunID.load(std::memory_order_relaxed);
        size_t taskCount = tasks.size();

        while (true)
        {
            size_t id = nextTask.fetch_add(1, std::memory_order_relaxed);

            if (id >= taskCount)
                break;

            tasks[id](threadID);
        }

        finished.fetch_add(1, std::memory_order_relaxed);
    }
}