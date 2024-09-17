#include <sys/sysinfo.h>
#include <sys/utsname.h>
#include <iostream>

void GetProcessors()
{
    std::cout << "Processors: " << get_nprocs_conf();
}

void GetLinuxVersion()
{
    struct utsname unameData;
    if (uname(&unameData) != -1)
    {
        std::cout << "OS: " << unameData.sysname << ' ' << unameData.release << std::endl;
    }
    else
    {
        std::cout << "You can't get os info" << std::endl;
    }
}

auto BytesToMegabytes(auto bytes)
{
    auto megabytes = bytes / 1024 / 1024;
    return megabytes;
}

void GetRAM()
{
    struct sysinfo info;
    if (sysinfo(&info) != -1)
    {
        std::cout << "OS: " << BytesToMegabytes(info.freeram) << "MB / " << BytesToMegabytes(info.totalram) << "MB" << std::endl;
    }
    else
    {
        std::cout << "You can't get os info" << std::endl;
    }
}

int main()
{
    GetLinuxVersion();
    GetRAM();
    GetProcessors();
}