#include "system_info.h"
#include <stdio.h>
#include <windows.h>
#include <processthreadsapi.h>
#include <psapi.h>
#include <pdh.h>

// Total virtual memory available
unsigned long long get_virtual_memory()
{
    MEMORYSTATUSEX virtualMemory;
    virtualMemory.dwLength = sizeof(MEMORYSTATUSEX);
    GlobalMemoryStatusEx(&virtualMemory);
    return virtualMemory.ullAvailPageFile;
}

// Virtual memory currently used
unsigned long long get_virtual_memory_usage()
{
    MEMORYSTATUSEX virtualMemoryUsage;
    virtualMemoryUsage.dwLength = sizeof(virtualMemoryUsage);
    GlobalMemoryStatusEx(&virtualMemoryUsage);

    DWORD vmUsage = virtualMemoryUsage.ullTotalPageFile - virtualMemoryUsage.ullAvailPageFile;
    return vmUsage;
}

// Virtual memory currently used by process
unsigned long long get_virtual_memory_process()
{
    HANDLE hProcess = OpenProcess(PROCESS_QUERY_INFORMATION | PROCESS_VM_READ, FALSE, GetCurrentProcessId()); // we need a handle to call GetProcessMemoryInfo()
    SIZE_T virtual_memory_process = 0;
    if (hProcess == NULL)
    {
        return 0;
    }
    PROCESS_MEMORY_COUNTERS pmc;
    if (GetProcessMemoryInfo(hProcess, &pmc, sizeof(pmc)))
    {
        virtual_memory_process = pmc.PagefileUsage;
    }
    else
    {
        return 0;
    }

    CloseHandle(hProcess);
    return virtual_memory_process;
}

// Total RAM available
unsigned long long get_ram()
{
    MEMORYSTATUSEX ram;
    ram.dwLength = sizeof(ram);
    GlobalMemoryStatusEx(&ram);
    return ram.ullAvailPhys;
}

// RAM currently used
unsigned long long get_ram_usage()
{
    MEMORYSTATUSEX ram;
    ram.dwLength = sizeof(ram);
    GlobalMemoryStatusEx(&ram);
    DWORD ramUsage = ram.ullTotalPhys - ram.ullAvailPhys;

    return ramUsage;
}

// RAM currently used by the process
unsigned long long get_ram_from_process()
{
    HANDLE hProcess = OpenProcess(PROCESS_QUERY_INFORMATION | PROCESS_VM_READ, FALSE, GetCurrentProcessId()); // we need a handle to call GetProcessMemoryInfo()
    SIZE_T ram_usage_process = 0;
    if (hProcess == NULL)
    {
        return 0;
    }
    PROCESS_MEMORY_COUNTERS pmc;
    if (GetProcessMemoryInfo(hProcess, &pmc, sizeof(pmc)))
    {
        ram_usage_process = pmc.WorkingSetSize;
    }
    else
    {
        return 0;
    }

    CloseHandle(hProcess);
    return ram_usage_process;
}

// Percentage of CPU usage using performance counters
double percent_cpu_usage()
{
    HQUERY query;
    HCOUNTER counter;
    PDH_FMT_COUNTERVALUE counterValue;

    // Open query
    if (PdhOpenQuery(NULL, 0, &query) != ERROR_SUCCESS)
    {
        return 0.0;
    }

    // Add CPU usage counter
    if (PdhAddCounterW(query, L"\\Processor(_Total)\\% Processor Time", 0, &counter) != ERROR_SUCCESS)
    {
        PdhCloseQuery(query);
        return 0.0;
    }

    // Collect query data
    if (PdhCollectQueryData(query) != ERROR_SUCCESS)
    {
        PdhCloseQuery(query);
        return 0.0;
    }

    // Sleep for a short interval to allow for data collection
    Sleep(1000);

    // Collect query data again
    if (PdhCollectQueryData(query) != ERROR_SUCCESS)
    {
        PdhCloseQuery(query);
        return 0.0;
    }

    // Get formatted counter value
    if (PdhGetFormattedCounterValue(counter, PDH_FMT_DOUBLE, NULL, &counterValue) != ERROR_SUCCESS)
    {
        PdhCloseQuery(query);
        return 0.0;
    }

    // Close query
    PdhCloseQuery(query);

    return counterValue.doubleValue;
}

// Percentage of CPU usage by process
double percent_cpu_process()
{
    static ULARGE_INTEGER lastCPU, lastSysIdle, lastSysKernel, lastSysUser;
    static int numProcessors = -1;
    FILETIME ftime, fsysIdle, fsysKernel, fsysUser;
    ULARGE_INTEGER now, sysIdle, sysKernel, sysUser;
    double percent = 0.0;

    if (numProcessors == -1) // Initialization check
    {
        SYSTEM_INFO sysInfo;

        GetSystemInfo(&sysInfo);
        numProcessors = sysInfo.dwNumberOfProcessors;

        GetSystemTimeAsFileTime(&ftime);
        memcpy(&lastCPU, &ftime, sizeof(FILETIME));

        GetSystemTimes(&fsysIdle, &fsysKernel, &fsysUser);
        memcpy(&lastSysIdle, &fsysIdle, sizeof(FILETIME));
        memcpy(&lastSysKernel, &fsysKernel, sizeof(FILETIME));
        memcpy(&lastSysUser, &fsysUser, sizeof(FILETIME));
    }

    // Sleep before collecting data to ensure meaningful values
    Sleep(1000);

    GetSystemTimeAsFileTime(&ftime);
    memcpy(&now, &ftime, sizeof(FILETIME));

    if (!GetSystemTimes(&fsysIdle, &fsysKernel, &fsysUser))
    {
        return 0.0;
    }
    memcpy(&sysIdle, &fsysIdle, sizeof(FILETIME));
    memcpy(&sysKernel, &fsysKernel, sizeof(FILETIME));
    memcpy(&sysUser, &fsysUser, sizeof(FILETIME));

    ULONGLONG idleDiff = sysIdle.QuadPart - lastSysIdle.QuadPart;
    ULONGLONG kernelDiff = sysKernel.QuadPart - lastSysKernel.QuadPart;
    ULONGLONG userDiff = sysUser.QuadPart - lastSysUser.QuadPart;
    ULONGLONG totalDiff = kernelDiff + userDiff;

    if (totalDiff > 0)
    {
        percent = (100.0 * (kernelDiff + userDiff - idleDiff)) / totalDiff;
    }

    lastCPU = now;
    lastSysIdle = sysIdle;
    lastSysKernel = sysKernel;
    lastSysUser = sysUser;

    return percent;
}
