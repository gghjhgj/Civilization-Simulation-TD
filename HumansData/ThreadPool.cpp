#include "ThreadPool.hpp"
#include <iostream>

ThreadPool::ThreadPool(size_t count)
{
    std::cout << "Creating " << count << " spinning threads (No OS sleep overhead)" << std::endl;
    workers.reserve(count);
    queues.reserve(count);

    for (size_t i = 0; i < count; i++)
    {
        queues.emplace_back(std::make_unique<WorkerQueue>());
    }
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
    tasks = &newTasks;

    finished.store(0, std::memory_order_relaxed);

    size_t totalTasks = newTasks.size();
    size_t perWorker = (totalTasks + workers.size() - 1) / workers.size();

    for (size_t i = 0; i < workers.size(); i++)
    {
        size_t begin = i * perWorker;
        size_t end = std::min(begin + perWorker, totalTasks);

        queues[i]->begin = begin;
        queues[i]->end = end;
        queues[i]->current.store(begin, std::memory_order_relaxed);
    }

    currentRunID.fetch_add(1, std::memory_order_release);


    int spins = 0;

    while (finished.load(std::memory_order_relaxed) < workers.size())
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

        size_t runID;

        while ((runID = currentRunID.load(std::memory_order_relaxed)) == localRunID)
        {
            cpuRelax(spins);
        }

        if (stop.load(std::memory_order_relaxed))
            break;


        localRunID = runID;


        auto* localTasks = tasks;
        size_t workerCount = workers.size();

        while (true)
        {
            size_t id = queues[threadID]->current.fetch_add(1, std::memory_order_relaxed);

            if (id >= queues[threadID]->end)
                break;

            (*localTasks)[id](threadID);
        }

        for (size_t i = 0;i < workerCount;i++)
        {
            if (i == threadID)
                continue;

            size_t stealBegin = queues[i]->current.load(std::memory_order_relaxed);
            size_t stealEnd = queues[i]->end;

            while (stealBegin < stealEnd)
            {
                if (queues[i]->current.compare_exchange_weak(
                    stealBegin,
                    stealBegin + 1,
                    std::memory_order_relaxed))
                {
                    (*localTasks)[stealBegin](threadID);
                }
            }
        }
        finished.fetch_add(1, std::memory_order_relaxed);
    }
}