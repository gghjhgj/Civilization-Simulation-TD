#pragma once

#ifdef _WIN32
#include "ThreadController.hpp"
#include <tbb/task_scheduler_observer.h>

class TBBPinObserver :
    public tbb::task_scheduler_observer
{

public:

    TBBPinObserver(
        tbb::task_arena& arena
    )
        :
        tbb::task_scheduler_observer(arena)
    {
        std::cout << "Observer created\n";
        observe(true);
    }


    void on_scheduler_entry(bool worker) override
    {

        if(!worker)
            return;


        int id =
            workerCounter.fetch_add(1);

        if(id >= 10)
            return;

        int cpu = 2 + id;


        DWORD_PTR mask =
            (DWORD_PTR)1 << cpu;


        SetThreadAffinityMask(
            GetCurrentThread(),
            mask
        );


        std::cout
            << "TBB worker "
            << id
            << " pinned CPU "
            << cpu
            << std::endl;
    }


private:

    std::atomic<int> workerCounter{0};

};
#endif