#pragma once

#ifdef _WIN32

#include <windows.h>
#include <iostream>
#include <vector>


inline void pinThread(int core)
{
    DWORD_PTR mask = (DWORD_PTR)1 << core;

    HANDLE thread = GetCurrentThread();

    SetThreadAffinityMask(thread, mask);
}

inline void pinPhysicalCore(int core)
{
#ifdef _WIN32

    DWORD_PTR mask = 0;

    int firstLogical = core * 2;

    mask |= (DWORD_PTR)1 << firstLogical;
    mask |= (DWORD_PTR)1 << (firstLogical + 1);

    SetThreadAffinityMask(
        GetCurrentThread(),
        mask
    );

#endif
}

inline void printCurrentCPU()
{
    std::cout
        << "Current logical CPU: "
        << GetCurrentProcessorNumber()
        << "\n";
}

inline void printCPUTopology()
{
    DWORD length = 0;
    GetLogicalProcessorInformationEx(
        RelationProcessorCore,
        nullptr,
        &length
    );
    if (length == 0)
    {
        std::cout << "Cannot get CPU topology\n";
        return;
    }
    std::vector<unsigned char> buffer(length);
    auto info =
        reinterpret_cast<PSYSTEM_LOGICAL_PROCESSOR_INFORMATION_EX>(
            buffer.data()
        );
    if (!GetLogicalProcessorInformationEx(
        RelationProcessorCore,
        info,
        &length))
    {
        std::cout << "Cannot get CPU topology\n";
        return;
    }
    DWORD offset = 0;
    int physicalCore = 0;
    while (offset < length)
    {
        auto current =
            reinterpret_cast<PSYSTEM_LOGICAL_PROCESSOR_INFORMATION_EX>(
                buffer.data() + offset
            );
        if (current->Relationship == RelationProcessorCore)
        {
            std::cout
                << "Physical core "
                << physicalCore++
                << " -> logical CPUs: ";
            for (WORD group = 0;
                 group < current->Processor.GroupCount;
                 group++)
            {
                KAFFINITY mask =
                    current->Processor.GroupMask[group].Mask;


                for (int cpu = 0;
                     cpu < sizeof(KAFFINITY) * 8;
                     cpu++)
                {
                    if (mask & ((KAFFINITY)1 << cpu))
                    {
                        std::cout << cpu << " ";
                    }
                }
            }
            std::cout << "\n";
        }
        offset += current->Size;
    }
}

#endif