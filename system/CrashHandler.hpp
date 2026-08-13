#pragma once

#ifdef _WIN32

#include <windows.h>
#include <dbghelp.h>

#include <iostream>
#include <sstream>
#include <string>

#pragma comment(lib, "dbghelp.lib")

namespace CrashHandler
{
    inline std::string hexAddress(DWORD64 address)
    {
        std::ostringstream stream;

        stream
            << "0x"
            << std::hex
            << std::uppercase
            << address;

        return stream.str();
    }

    inline std::string getModuleName(
        HANDLE process,
        DWORD64 address)
    {
        IMAGEHLP_MODULE64 moduleInfo{};

        moduleInfo.SizeOfStruct =
            sizeof(IMAGEHLP_MODULE64);

        if (SymGetModuleInfo64(
                process,
                address,
                &moduleInfo))
        {
            if (moduleInfo.ModuleName[0] != '\0')
            {
                return moduleInfo.ModuleName;
            }

            if (moduleInfo.ImageName[0] != '\0')
            {
                return moduleInfo.ImageName;
            }
        }

        return "UnknownModule";
    }

    inline void printRegisters(CONTEXT* ctx)
    {
        std::cerr
            << "\n--- CPU REGISTERS ---\n";

#ifdef _WIN64

        std::cerr
            << "RAX = " << hexAddress(ctx->Rax) << "\n"
            << "RBX = " << hexAddress(ctx->Rbx) << "\n"
            << "RCX = " << hexAddress(ctx->Rcx) << "\n"
            << "RDX = " << hexAddress(ctx->Rdx) << "\n"
            << "RSI = " << hexAddress(ctx->Rsi) << "\n"
            << "RDI = " << hexAddress(ctx->Rdi) << "\n"
            << "RBP = " << hexAddress(ctx->Rbp) << "\n"
            << "RSP = " << hexAddress(ctx->Rsp) << "\n"
            << "R8  = " << hexAddress(ctx->R8) << "\n"
            << "R9  = " << hexAddress(ctx->R9) << "\n"
            << "R10 = " << hexAddress(ctx->R10) << "\n"
            << "R11 = " << hexAddress(ctx->R11) << "\n"
            << "R12 = " << hexAddress(ctx->R12) << "\n"
            << "R13 = " << hexAddress(ctx->R13) << "\n"
            << "R14 = " << hexAddress(ctx->R14) << "\n"
            << "R15 = " << hexAddress(ctx->R15) << "\n"
            << "RIP = " << hexAddress(ctx->Rip) << "\n"
            << "EFLAGS = " << hexAddress(ctx->EFlags) << "\n";

#else

        std::cerr
            << "EAX = " << hexAddress(ctx->Eax) << "\n"
            << "EBX = " << hexAddress(ctx->Ebx) << "\n"
            << "ECX = " << hexAddress(ctx->Ecx) << "\n"
            << "EDX = " << hexAddress(ctx->Edx) << "\n"
            << "ESI = " << hexAddress(ctx->Esi) << "\n"
            << "EDI = " << hexAddress(ctx->Edi) << "\n"
            << "EBP = " << hexAddress(ctx->Ebp) << "\n"
            << "ESP = " << hexAddress(ctx->Esp) << "\n"
            << "EIP = " << hexAddress(ctx->Eip) << "\n"
            << "EFLAGS = " << hexAddress(ctx->EFlags) << "\n";

#endif

        std::cerr
            << "----------------------\n";
    }

    inline void printAccessViolation(
        EXCEPTION_RECORD* exception)
    {
        if (
            exception->ExceptionCode
            != EXCEPTION_ACCESS_VIOLATION)
        {
            return;
        }

        std::cerr
            << "\n--- ACCESS VIOLATION ---\n";

        if (exception->NumberParameters >= 2)
        {
            const ULONG_PTR operation =
                exception->ExceptionInformation[0];

            const ULONG_PTR address =
                exception->ExceptionInformation[1];

            switch (operation)
            {
                case 0:
                    std::cerr
                        << "Operation: READ\n";
                    break;

                case 1:
                    std::cerr
                        << "Operation: WRITE\n";
                    break;

                case 8:
                    std::cerr
                        << "Operation: EXECUTE\n";
                    break;

                default:
                    std::cerr
                        << "Operation: UNKNOWN ("
                        << operation
                        << ")\n";
                    break;
            }

            std::cerr
                << "Bad address: "
                << hexAddress(
                       static_cast<DWORD64>(
                           address))
                << "\n";
        }

        std::cerr
            << "------------------------\n";
    }

    inline void printStackTrace(
        CONTEXT* ctx)
    {
        HANDLE process =
            GetCurrentProcess();

        HANDLE thread =
            GetCurrentThread();

        SymSetOptions(
            SYMOPT_LOAD_LINES |
            SYMOPT_UNDNAME |
            SYMOPT_DEFERRED_LOADS |
            SYMOPT_FAIL_CRITICAL_ERRORS |
            SYMOPT_NO_PROMPTS
        );

        if (!SymInitialize(
                process,
                nullptr,
                TRUE))
        {
            std::cerr
                << "\n[CrashHandler] "
                << "SymInitialize failed. Error: "
                << GetLastError()
                << "\n";

            return;
        }

        STACKFRAME64 frame{};

        DWORD machineType;

#ifdef _WIN64

        machineType =
            IMAGE_FILE_MACHINE_AMD64;

        frame.AddrPC.Offset =
            ctx->Rip;

        frame.AddrPC.Mode =
            AddrModeFlat;

        frame.AddrStack.Offset =
            ctx->Rsp;

        frame.AddrStack.Mode =
            AddrModeFlat;

        frame.AddrFrame.Offset =
            ctx->Rbp;

        frame.AddrFrame.Mode =
            AddrModeFlat;

#else

        machineType =
            IMAGE_FILE_MACHINE_I386;

        frame.AddrPC.Offset =
            ctx->Eip;

        frame.AddrPC.Mode =
            AddrModeFlat;

        frame.AddrStack.Offset =
            ctx->Esp;

        frame.AddrStack.Mode =
            AddrModeFlat;

        frame.AddrFrame.Offset =
            ctx->Ebp;

        frame.AddrFrame.Mode =
            AddrModeFlat;

#endif

        std::cerr
            << "\n--- STACK TRACE ---\n";

        alignas(SYMBOL_INFO)
        char symbolBuffer[
            sizeof(SYMBOL_INFO)
            + MAX_SYM_NAME
        ];

        PSYMBOL_INFO symbol =
            reinterpret_cast<PSYMBOL_INFO>(
                symbolBuffer);

        symbol->SizeOfStruct =
            sizeof(SYMBOL_INFO);

        symbol->MaxNameLen =
            MAX_SYM_NAME;

        IMAGEHLP_LINE64 line{};

        line.SizeOfStruct =
            sizeof(IMAGEHLP_LINE64);

        DWORD displacementLine = 0;

        int frameCount = 0;

        while (true)
        {
            BOOL result =
                StackWalk64(
                    machineType,
                    process,
                    thread,
                    &frame,
                    ctx,
                    nullptr,
                    SymFunctionTableAccess64,
                    SymGetModuleBase64,
                    nullptr
                );

            if (!result)
            {
                std::cerr
                    << "[StackWalk64 failed] Error: "
                    << GetLastError()
                    << "\n";

                break;
            }

            if (frame.AddrPC.Offset == 0)
            {
                break;
            }

            const DWORD64 address =
                frame.AddrPC.Offset;

            std::cerr
                << "["
                << frameCount
                << "] ";

            const std::string module =
                getModuleName(
                    process,
                    address);

            std::cerr
                << "[" << module << "] ";

            DWORD64 displacementSym = 0;

            if (SymFromAddr(
                    process,
                    address,
                    &displacementSym,
                    symbol))
            {
                std::cerr
                    << symbol->Name;

                if (displacementSym != 0)
                {
                    std::cerr
                        << " + "
                        << hexAddress(
                               displacementSym);
                }
            }
            else
            {
                std::cerr
                    << "?? (Unknown Function)";

                DWORD64 moduleBase =
                    SymGetModuleBase64(
                        process,
                        address);

                if (moduleBase != 0)
                {
                    std::cerr
                        << " + "
                        << hexAddress(
                               address -
                               moduleBase);
                }
            }

            if (SymGetLineFromAddr64(
                    process,
                    address,
                    &displacementLine,
                    &line))
            {
                std::cerr
                    << " | "
                    << line.FileName
                    << ":"
                    << line.LineNumber;

                if (displacementLine != 0)
                {
                    std::cerr
                        << " +"
                        << displacementLine;
                }
            }
            else
            {
                std::cerr
                    << " | line unavailable";
            }

            std::cerr
                << " | "
                << hexAddress(address)
                << "\n";

            frameCount++;

            if (frameCount >= 30)
            {
                std::cerr
                    << "[Stack trace truncated after 30 frames]\n";

                break;
            }
        }

        std::cerr
            << "--------------------\n";

        SymCleanup(process);
    }

    inline LONG WINAPI crashHandler(
        EXCEPTION_POINTERS* e)
    {
        std::cerr
            << "\n"
            << "========================================\n"
            << "          FATAL CRASH\n"
            << "========================================\n";

        if (e == nullptr)
        {
            std::cerr
                << "EXCEPTION POINTERS: NULL\n";

            return EXCEPTION_EXECUTE_HANDLER;
        }

        EXCEPTION_RECORD* exception =
            e->ExceptionRecord;

        CONTEXT* ctx =
            e->ContextRecord;

        std::cerr
            << "EXCEPTION CODE: "
            << hexAddress(
                   exception->ExceptionCode)
            << "\n";

        switch (
            exception->ExceptionCode)
        {
            case EXCEPTION_ACCESS_VIOLATION:
                std::cerr
                    << "REASON: Access Violation\n";
                break;

            case EXCEPTION_STACK_OVERFLOW:
                std::cerr
                    << "REASON: Stack Overflow\n";
                break;

            case EXCEPTION_INT_DIVIDE_BY_ZERO:
                std::cerr
                    << "REASON: Integer Divide By Zero\n";
                break;

            case EXCEPTION_ILLEGAL_INSTRUCTION:
                std::cerr
                    << "REASON: Illegal Instruction\n";
                break;

            case EXCEPTION_ARRAY_BOUNDS_EXCEEDED:
                std::cerr
                    << "REASON: Array Bounds Exceeded\n";
                break;

            case EXCEPTION_DATATYPE_MISALIGNMENT:
                std::cerr
                    << "REASON: Datatype Misalignment\n";
                break;

            case EXCEPTION_IN_PAGE_ERROR:
                std::cerr
                    << "REASON: In-Page Error\n";
                break;

            default:
                std::cerr
                    << "REASON: Unknown Exception\n";
                break;
        }

        std::cerr
            << "FAULT ADDRESS: "
            << exception->ExceptionAddress
            << "\n";

        printAccessViolation(
            exception);

        if (ctx != nullptr)
        {
            printRegisters(ctx);
            printStackTrace(ctx);
        }

        std::cerr
            << "\n"
            << "========================================\n"
            << "        END OF CRASH REPORT\n"
            << "========================================\n"
            << std::flush;

        return EXCEPTION_EXECUTE_HANDLER;
    }
}

#endif