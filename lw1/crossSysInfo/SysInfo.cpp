#include "SysInfo.h"

#ifdef _WIN32
#include <Windows.h>
#include <VersionHelpers.h>
#else
#include <sys/sysinfo.h>
#include <sys/utsname.h>
#endif

//сделать меньше ifdef

auto BytesToMegabytes(auto bytes)
{
    auto megabytes = bytes / 1024 / 1024;
    return megabytes;
}

#ifdef  _WIN32
std::string SysInfo::GetOSName()
{
    return "Windows";
}

std::string SysInfo::GetOSVersion()
{
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
}

uint64_t SysInfo::GetFreeMemory()
{
    MEMORYSTATUSEX memoryInfo;
    memoryInfo.dwLength = sizeof(memoryInfo);
    if (GlobalMemoryStatusEx(&memoryInfo))
    {
        return BytesToMegabytes(memoryInfo.ullAvailPhys);
    }
    return -1;
}

uint64_t SysInfo::GetTotalMemory()
{
    MEMORYSTATUSEX memoryInfo;
    memoryInfo.dwLength = sizeof(memoryInfo);
    if (GlobalMemoryStatusEx(&memoryInfo))
    {
        return BytesToMegabytes(memoryInfo.ullTotalPhys);
    }
    return -1;
}

unsigned SysInfo::GetProcessorCount()
{
    SYSTEM_INFO sysInfo;
    GetSystemInfo(&sysInfo);
    return sysInfo.dwNumberOfProcessors;
}
#else
std::string SysInfo::GetOSName()
{
    return "Linux";
}

std::string SysInfo::GetOSVersion()
{
    struct utsname unameInfo;
    uname(&unameInfo);
    return std::string(unameInfo.release);
}

uint64_t SysInfo::GetFreeMemory()
{
    struct sysinfo memoryInfo;
    if (sysinfo(&memoryInfo) != -1)
    {
        return BytesToMegabytes(memoryInfo.freeram);
    }
    return -1;
}

uint64_t SysInfo::GetTotalMemory()
{
    struct sysinfo memoryInfo;
    if (sysinfo(&memoryInfo) != -1)
    {
        return BytesToMegabytes(memoryInfo.totalram);
    }
    return -1;
}

unsigned SysInfo::GetProcessorCount()
{
    return get_nprocs_conf();
}
#endif
