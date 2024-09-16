#include <windows.h>
#include <VersionHelpers.h>
#include <iostream>

void GetWindowsVersion()
{
    std::cout << "OS: ";
    if (IsWindows10OrGreater())
    {
        std::cout << "Windows 10 or Greater" << std::endl;
    }
    else if (IsWindows8OrGreater())
    {
        std::cout << "Windows 8" << std::endl;
    }
    else if (IsWindows7OrGreater())
    {
        std::cout << "Windows 7" << std::endl;
    }
    else if (IsWindowsVistaOrGreater())
    {
        std::cout << "Windows Vista" << std::endl;
    }
    else if (IsWindowsXPOrGreater())
    {
        std::cout << "Windows XP" << std::endl;
    }
    else if (IsWindowsServer())
    {
        std::cout << "Windows Server" << std::endl;
    }
}

auto BytesToMegabytes(auto bytes)
{
    auto megabytes = bytes / 1024 / 1024;
    return megabytes;
}

void GetRAM()
{
    MEMORYSTATUSEX statex;
    statex.dwLength = sizeof (statex);
    if (GlobalMemoryStatusEx(&statex))
    {
        auto availableMemory = BytesToMegabytes(statex.ullAvailPhys);
        auto totalMemory = BytesToMegabytes(statex.ullTotalPhys);
        std::cout << "RAM: " << availableMemory << "MB / " << totalMemory << "MB" << std::endl;
    }
    else
    {
        std::cout << "You can't get memory info" << std::endl;
    }
}

void GetProcessors()
{
    SYSTEM_INFO systemInfo;
    GetSystemInfo(&systemInfo);
    auto processorsCount = systemInfo.dwNumberOfProcessors;
    std::cout << "Processors: " << processorsCount << std::endl;
}

int main()
{
    GetWindowsVersion();
    GetRAM();
    GetProcessors();
}
