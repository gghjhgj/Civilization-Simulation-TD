#pragma once

#ifdef _WIN32

#include <windows.h>

#include <iostream>
#include <vector>
#include <stdexcept>
#include <cstdint>

#include "../Config/Config.h"


struct CPUTopology
{
    int physicalCores = 0;
    int logicalThreads = 0;

    // Każdy element odpowiada jednemu fizycznemu rdzeniowi.
    //
    // Przykład Ryzen 5 5500U:
    //
    // physicalCoreMasks[0] -> CPU 0 + CPU 6
    // physicalCoreMasks[1] -> CPU 1 + CPU 7
    // physicalCoreMasks[2] -> CPU 2 + CPU 8
    // ...
    std::vector<DWORD_PTR> physicalCoreMasks;

    bool hasSMT = false;
};


inline CPUTopology getCPUTopology()
{
    CPUTopology topology;

    DWORD length = 0;

    GetLogicalProcessorInformationEx(
        RelationProcessorCore,
        nullptr,
        &length
    );

    if (length == 0)
    {
        throw std::runtime_error(
            "Cannot get CPU topology"
        );
    }


    std::vector<unsigned char> buffer(length);


    auto* info =
        reinterpret_cast<
            PSYSTEM_LOGICAL_PROCESSOR_INFORMATION_EX
        >(buffer.data());


    if (!GetLogicalProcessorInformationEx(
            RelationProcessorCore,
            info,
            &length))
    {
        throw std::runtime_error(
            "Cannot get CPU topology"
        );
    }


    DWORD offset = 0;


    while (offset < length)
    {
        auto* current =
            reinterpret_cast<
                PSYSTEM_LOGICAL_PROCESSOR_INFORMATION_EX
            >(buffer.data() + offset);


        if (current->Relationship ==
            RelationProcessorCore)
        {
            topology.physicalCores++;


            DWORD_PTR coreMask = 0;


            for (WORD group = 0;
                 group < current->Processor.GroupCount;
                 ++group)
            {
                coreMask |=
                    static_cast<DWORD_PTR>(
                        current->Processor.GroupMask[group].Mask
                    );
            }


            topology.physicalCoreMasks.push_back(
                coreMask
            );


            int logicalCount = 0;


            for (int bit = 0;
                 bit < static_cast<int>(
                     sizeof(DWORD_PTR) * 8);
                 ++bit)
            {
                if (coreMask &
                    (static_cast<DWORD_PTR>(1) << bit))
                {
                    logicalCount++;
                }
            }


            topology.logicalThreads += logicalCount;


            if (logicalCount > 1)
            {
                topology.hasSMT = true;
            }
        }


        offset += current->Size;
    }


    return topology;
}


inline void printCPUTopology()
{
    const CPUTopology topology =
        getCPUTopology();


    std::cout
        << "\n========== CPU TOPOLOGY ==========\n";


    std::cout
        << "Physical cores: "
        << topology.physicalCores
        << "\n";


    std::cout
        << "Logical threads: "
        << topology.logicalThreads
        << "\n";


    std::cout
        << "SMT / Hyper-Threading: "
        << (topology.hasSMT ? "YES" : "NO")
        << "\n";


    for (size_t core = 0;
         core < topology.physicalCoreMasks.size();
         ++core)
    {
        const DWORD_PTR mask =
            topology.physicalCoreMasks[core];


        std::cout
            << "Physical core "
            << core
            << " -> logical CPUs: ";


        for (int bit = 0;
             bit < static_cast<int>(
                 sizeof(DWORD_PTR) * 8);
             ++bit)
        {
            if (mask &
                (static_cast<DWORD_PTR>(1) << bit))
            {
                std::cout << bit << " ";
            }
        }


        std::cout << "\n";
    }


    std::cout
        << "==================================\n\n";
}


inline std::vector<int> getLogicalCPUsFromMask(
    DWORD_PTR mask)
{
    std::vector<int> cpus;


    for (int bit = 0;
         bit < static_cast<int>(
             sizeof(DWORD_PTR) * 8);
         ++bit)
    {
        if (mask &
            (static_cast<DWORD_PTR>(1) << bit))
        {
            cpus.push_back(bit);
        }
    }


    return cpus;
}


inline std::vector<int> getHumanLoopCPUs()
{
    const CPUTopology topology =
        getCPUTopology();


    std::vector<int> cpus;


    /*
        Physical core 0 jest zarezerwowany
        dla simulation loop.

        Wszystkie pozostałe rdzenie idą
        do Human/TBB.
    */

    for (size_t core = 1;
         core < topology.physicalCoreMasks.size();
         ++core)
    {
        const DWORD_PTR mask =
            topology.physicalCoreMasks[core];


        const std::vector<int> coreCPUs =
            getLogicalCPUsFromMask(mask);


        cpus.insert(
            cpus.end(),
            coreCPUs.begin(),
            coreCPUs.end()
        );
    }


    return cpus;
}


inline DWORD_PTR getSimulationLoopMask()
{
    const CPUTopology topology =
        getCPUTopology();


    if (topology.physicalCoreMasks.empty())
    {
        throw std::runtime_error(
            "No physical CPU cores detected."
        );
    }


    return topology.physicalCoreMasks[0];
}


inline void pinThreadToMask(
    DWORD_PTR mask)
{
    if (mask == 0)
    {
        throw std::runtime_error(
            "Cannot pin thread: CPU mask is zero."
        );
    }


    if (SetThreadAffinityMask(
            GetCurrentThread(),
            mask) == 0)
    {
        throw std::runtime_error(
            "SetThreadAffinityMask failed."
        );
    }
}


inline void pinThread(int logicalCPU)
{
    if (logicalCPU < 0)
    {
        throw std::runtime_error(
            "Invalid logical CPU."
        );
    }


    const DWORD_PTR mask =
        static_cast<DWORD_PTR>(1)
        << logicalCPU;


    pinThreadToMask(mask);
}


inline void pinPhysicalCore(int core)
{
    const CPUTopology topology =
        getCPUTopology();


    if (core < 0 ||
        core >= static_cast<int>(
            topology.physicalCoreMasks.size()))
    {
        throw std::runtime_error(
            "Invalid physical core."
        );
    }


    pinThreadToMask(
        topology.physicalCoreMasks[core]
    );
}


inline void configureThreadsAutomatically()
{
    const CPUTopology topology =
        getCPUTopology();


    if (topology.physicalCores < 2)
    {
        throw std::runtime_error(
            "At least 2 physical CPU cores are required."
        );
    }


    /*
        CORE 0
        ------
        Simulation loop.
    */

    Config::threads.coresForSimLoop = 1;


    Config::threads.threadsForSimLoop =
        static_cast<int>(
            getLogicalCPUsFromMask(
                topology.physicalCoreMasks[0]
            ).size()
        );


    /*
        CORES 1..N
        ----------
        Human loop.
    */

    Config::threads.coresForHumanLoop =
        topology.physicalCores - 1;


    Config::threads.threadsForHumanLoop =
        topology.logicalThreads -
        Config::threads.threadsForSimLoop;


    /*
        Informacyjnie aktualizujemy również
        całkowitą liczbę wykrytych rdzeni/wątków.
    */

    Config::threads.cores =
        topology.physicalCores;

    Config::threads.threads =
        topology.logicalThreads;


    std::cout
        << "\n========== AUTO THREAD CONFIG ==========\n";


    std::cout
        << "Detected physical cores: "
        << topology.physicalCores
        << "\n";


    std::cout
        << "Detected logical threads: "
        << topology.logicalThreads
        << "\n";


    std::cout
        << "SMT / Hyper-Threading: "
        << (topology.hasSMT ? "YES" : "NO")
        << "\n";


    std::cout
        << "\nSimulation loop:\n"
        << "  physical cores = "
        << Config::threads.coresForSimLoop
        << "\n"
        << "  logical threads = "
        << Config::threads.threadsForSimLoop
        << "\n";


    std::cout
        << "\nHuman loop:\n"
        << "  physical cores = "
        << Config::threads.coresForHumanLoop
        << "\n"
        << "  logical threads = "
        << Config::threads.threadsForHumanLoop
        << "\n";


    std::cout
        << "========================================\n\n";
}


inline void printCurrentCPU()
{
    std::cout
        << "Current logical CPU: "
        << GetCurrentProcessorNumber()
        << "\n";
}

#endif