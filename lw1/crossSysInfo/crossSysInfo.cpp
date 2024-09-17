#include "SysInfo.h"
#include <iostream>

int main()
{
    std::cout << "OS: " << SysInfo::GetOSName() << ' ' << SysInfo::GetOSVersion() << std::endl;
    if (SysInfo::GetFreeMemory() == -1 || SysInfo::GetTotalMemory() == -1)
    {
        std::cout << "Error! RAM is not available!";
    }
    else
    {
        std::cout << "RAM: " << SysInfo::GetFreeMemory() << "MB / " << SysInfo::GetTotalMemory() << "MB" << std::endl;
    }
    std::cout << "Processors: " << SysInfo::GetProcessorCount() << std::endl;
}