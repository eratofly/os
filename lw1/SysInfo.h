#ifndef OS_SYSINFO_H
#define OS_SYSINFO_H

#include <string>
#include <cstdio>
#include <cstdint>

class SysInfo
{
public:
    static std::string GetOSName();
    static std::string GetOSVersion();
    static uint64_t GetFreeMemory();
    static uint64_t GetTotalMemory();
    static unsigned GetProcessorCount();
};

#endif //OS_SYSINFO_H
