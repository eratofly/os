#include "SysInfo.h"

#ifdef _WIN32
#include <Windows.h>
#include <VersionHelpers.h>
#else
#include <sys/sysinfo.h>
#include <sys/utsname.h>
#endif

auto BytesToMegabytes(auto bytes)
{
    auto megabytes = bytes / 1024 / 1024;
    return megabytes;
}

std::string SysInfo::GetOSName() {
#ifdef _WIN32
    return "Windows";
#else
    return "Linux";
#endif
}

std::string SysInfo::GetOSVersion() {
#ifdef _WIN32
    if (IsWindows10OrGreater())
    {
        return "10 or Greater";
    }
    else if (IsWindows8OrGreater())
    {
        return "8";
    }
    else if (IsWindows7OrGreater())
    {
       return "7";
    }
    else if (IsWindowsVistaOrGreater())
    {
        return "Vista";
    }
    else if (IsWindowsXPOrGreater())
    {
        return "XP";
    }
    else if (IsWindowsServer())
    {
        return "Server";
    }
    return "Unknown Version";
#else
    struct utsname unameInfo;
    uname(&unameInfo);
    return std::string(unameInfo.release);
#endif
}

uint64_t SysInfo::GetFreeMemory() {
#ifdef _WIN32
    MEMORYSTATUSEX memoryInfo;
    memoryInfo.dwLength = sizeof(memoryInfo);
    if (GlobalMemoryStatusEx(&memoryInfo))
    {
        return BytesToMegabytes(memoryInfo.ullAvailPhys);
    }
    return -1;
#else
    struct sysinfo memoryInfo;
    if (sysinfo(&memoryInfo) != -1)
    {
        return BytesToMegabytes(memoryInfo.freeram);
    }
    return -1;
#endif
}

uint64_t SysInfo::GetTotalMemory() {
#ifdef _WIN32
    MEMORYSTATUSEX memoryInfo;
    memoryInfo.dwLength = sizeof(memoryInfo);
    if (GlobalMemoryStatusEx(&memoryInfo))
    {
        return BytesToMegabytes(memoryInfo.ullTotalPhys);
    }
    return -1;
#else
    struct sysinfo memoryInfo;
    if (sysinfo(&memoryInfo) != -1)
    {
        return BytesToMegabytes(memoryInfo.totalram);
    }
    return -1;
#endif
}

unsigned SysInfo::GetProcessorCount() {
#ifdef _WIN32
    SYSTEM_INFO sysInfo;
    GetSystemInfo(&sysInfo);
    return sysInfo.dwNumberOfProcessors;
#else
    return get_nprocs_conf();
#endif
}