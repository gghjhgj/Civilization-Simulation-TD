#pragma once

#ifdef _WIN32

#include <windows.h>
#include <iostream>

inline LONG WINAPI crashHandler(EXCEPTION_POINTERS* e)
{
    std::cerr << "\n========== CRASH ==========\n";

    std::cerr
        << "CODE: 0x"
        << std::hex
        << e->ExceptionRecord->ExceptionCode
        << "\n";

    std::cerr
        << "ADDRESS: "
        << e->ExceptionRecord->ExceptionAddress
        << "\n";


    CONTEXT* ctx = e->ContextRecord;

#ifdef _WIN64
    std::cerr
        << "RIP: 0x"
        << std::hex
        << ctx->Rip
        << "\n";

    std::cerr
        << "RSP: 0x"
        << ctx->Rsp
        << "\n";

    std::cerr
        << "RBP: 0x"
        << ctx->Rbp
        << "\n";
#endif


    std::cerr << "===========================\n";

    return EXCEPTION_EXECUTE_HANDLER;
}

#endif