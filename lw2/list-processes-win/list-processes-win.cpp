#include <windows.h>
#include <tlhelp32.h>
#include <iostream>
#include <iomanip>
#include <psapi.h>

int main()
{
    HANDLE hProcessSnap = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
    if (hProcessSnap == INVALID_HANDLE_VALUE)
    {
        return EXIT_FAILURE;
    }

    PROCESSENTRY32 pe32;
    pe32.dwSize = sizeof(PROCESSENTRY32);

    if (!Process32First(hProcessSnap, &pe32))
    {
        std::cerr << "Error: Unable to retrieve information about the first process" << std::endl;
        CloseHandle(hProcessSnap);
        return EXIT_FAILURE;
    }

    while (Process32Next(hProcessSnap, &pe32))
    {
        HANDLE hProcess = OpenProcess(PROCESS_QUERY_INFORMATION | PROCESS_VM_READ, FALSE, pe32.th32ProcessID);
        if (hProcess != NULL)
        {
            PROCESS_MEMORY_COUNTERS pmc;
            if (GetProcessMemoryInfo(hProcess, &pmc, sizeof(pmc)))
            {
                std::cout << pe32.th32ProcessID << "     " << pe32.szExeFile << "     " << pmc.WorkingSetSize / 1024 << " KB" << std::endl;
            } else
            {
                std::cerr << "Error: Unable to retrieve memory information for process " << pe32.szExeFile << std::endl;
            }
            CloseHandle(hProcess);
        } else
        {
            std::cerr << "Error: Unable to open process " << pe32.szExeFile << std::endl;
        }
    }

    do
    {
        HANDLE hProcess = OpenProcess(PROCESS_QUERY_INFORMATION | PROCESS_VM_READ, FALSE, pe32.th32ProcessID);
        if (hProcess != NULL)
        {
            PROCESS_MEMORY_COUNTERS pmc;
            if (GetProcessMemoryInfo(hProcess, &pmc, sizeof(pmc)))
            {
                std::cout << pe32.th32ProcessID << "     " << pe32.szExeFile << "     " << pmc.WorkingSetSize / 1024 << " KB" << std::endl;
            } else
            {
                std::cerr << "Error: Unable to retrieve memory information for process " << pe32.szExeFile << std::endl;
            }
            CloseHandle(hProcess);
        } else
        {
            std::cerr << "Error: Unable to open process " << pe32.szExeFile << std::endl;
        }
    } while (Process32Next(hProcessSnap, &pe32));

    CloseHandle(hProcessSnap);
    return 0;
}