#pragma once

#ifdef _WIN32
#include <windows.h>
#endif


inline void pinThread(int core)
{
#ifdef _WIN32
    DWORD_PTR mask = (DWORD_PTR)1 << core;

    HANDLE thread = GetCurrentThread();

    SetThreadAffinityMask(thread, mask);
#endif
}