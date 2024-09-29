//#include <windows.h>
//#include <unistd.h>
//#include <iostream>
//#include <tlhelp32.h>
//
//void GetPidProcess()
//{
//    std::cout << "Process pid: " << getpid() << std::endl;
//}
//
//void GetFileName()
//{
//    char buf[MAX_PATH];
//    GetModuleFileNameA(nullptr, buf, MAX_PATH);
//    std::cout << "Executable file name: " << buf << std::endl;
//}
//
//int main() {
//    PROCESSENTRY32  pe32 = {0};
//    HANDLE hsp = CreateToolhelp32Snapshot( TH32CS_SNAPPROCESS, 0 );
//
//    pe32.dwSize = sizeof( PROCESSENTRY32 );
//    Process32First( hsp, &pe32 );
//// access   PROCESSENTRY32 values
///*
//DWORD   dwSize;
//DWORD   cntUsage;
//DWORD   th32ProcessID;          // this process
//ULONG_PTR th32DefaultHeapID;
//DWORD   th32ModuleID;           // associated exe
//DWORD   cntThreads;
//DWORD   th32ParentProcessID;    // this process's parent process
//LONG    pcPriClassBase;         // Base priority of process's threads
//DWORD   dwFlags;
//CHAR    szExeFile[MAX_PATH];    // Path
//*/
//    while( Process32Next( hsp, &pe32 ) )
//    {
//        std::cout << pe32.szExeFile[MAX_PATH] << " ";
//        std::cout << pe32.th32ProcessID << std::endl;
//    }
//}

#include <windows.h>
#include <tlhelp32.h>
#include <iostream>
#include <psapi.h>

void ListProcesses() {
    std::cout << "List Processes" << std::endl;
    // Создаем снимок всех процессов
    DWORD TH32CS_PROCESS;
    HANDLE hSnapshot = CreateToolhelp32Snapshot(TH32CS_PROCESS, 0);
    if (hSnapshot == INVALID_HANDLE_VALUE) {
        std::cout << "Failed to create snapshot of processes." << std::endl;
        return;
    }
    std::cout << "List Processes 2" << std::endl;

    PROCESSENTRY32 pe;
    pe.dwSize = sizeof(PROCESSENTRY32);
    std::cout << Process32First(hSnapshot, &pe) << std::endl;
    // Получаем информацию о первом процессе
    if (Process32Next( hSnapshot, &pe )) {
        do {
            std::cout << "here" << std::endl;
            // Получаем информацию о памяти процесса
            HANDLE hProcess = OpenProcess(PROCESS_QUERY_INFORMATION | PROCESS_VM_READ, FALSE, pe.th32ProcessID);
            if (hProcess) {
                PROCESS_MEMORY_COUNTERS pmc;
                if (GetProcessMemoryInfo(hProcess, &pmc, sizeof(pmc))) {
                    std::cout << "PID: " << pe.th32ProcessID
                              << ", Name: " << pe.szExeFile
                              << ", Memory Usage: " << pmc.WorkingSetSize / 1024 << " KB"
                              << std::endl;
                }
                CloseHandle(hProcess);
            }
        } while (Process32Next(hSnapshot, &pe));
    }

    CloseHandle(hSnapshot);
}

int main() {
    ListProcesses();
    return 0;
}
