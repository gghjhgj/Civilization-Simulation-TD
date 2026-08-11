#pragma once

#ifdef _WIN32

#include "ThreadController.hpp"

#include <tbb/task_scheduler_observer.h>
#include <tbb/task_arena.h>

#include <windows.h>

#include <atomic>
#include <iostream>
#include <vector>
#include <utility>

class TBBPinObserver :
    public tbb::task_scheduler_observer
{
public:

    TBBPinObserver(
        tbb::task_arena& arena,
        std::vector<int> logicalCPUs
    )
        :
        tbb::task_scheduler_observer(arena),
        logicalCPUs(std::move(logicalCPUs))
    {
        observe(true);
    }

    void on_scheduler_entry(bool worker) override
    {
        if (!worker)
            return;

        if (logicalCPUs.empty())
            return;

        static thread_local bool alreadyPinned = false;

        if (alreadyPinned)
            return;

        const int workerID =
            workerCounter.fetch_add(
                1,
                std::memory_order_relaxed
            );

        if (
            workerID >=
            static_cast<int>(logicalCPUs.size())
        )
        {
            return;
        }

        const int cpu =
            logicalCPUs[workerID];

        const DWORD_PTR mask =
            static_cast<DWORD_PTR>(1) << cpu;

        if (
            SetThreadAffinityMask(
                GetCurrentThread(),
                mask
            ) == 0
        )
        {
            std::cerr
                << "ERROR: failed to pin TBB worker "
                << workerID
                << " to logical CPU "
                << cpu
                << "\n";

            return;
        }

        alreadyPinned = true;

        std::cout
            << "TBB worker "
            << workerID
            << " pinned to logical CPU "
            << cpu
            << "\n";
    }


private:

    std::atomic<int> workerCounter{0};
    std::vector<int> logicalCPUs;
};

#endif