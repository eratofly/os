#include <iostream>
#include <vector>
#include <unistd.h>
#include <signal.h>
#include <sys/wait.h>
#include <cstring>
#include <stdexcept>
#include <chrono>
#include <thread>

using namespace std::chrono;

class ProcessManager
{
public:
    pid_t SpawnChild()
    {
        pid_t pid = fork();
        if (pid < 0)
        {
            throw std::runtime_error("Failed to fork child process");
        }
        else if (pid == 0)
        {
            RunChildProcess();
            return 0;
        }
        else
        {
            childPids.push_back(pid);
        }
        return pid;
    }

    void KillChild()
    {
        if (HasChildren())
        {
            pid_t pid = childPids.back();
            if (KillProcess(pid))
            {
                std::cout << "Killed child process with PID " << pid << std::endl;
                childPids.pop_back();
            }
        }
        else
        {
            std::cout << "No children to kill" << std::endl;
        }
    }

    bool HasChildren() const
    {
        return !childPids.empty();
    }

private:
    std::vector<pid_t> childPids;

    static void RunChildProcess()
    {
        signal(SIGTERM, HandleSignal);
        signal(SIGINT, HandleSignal);

        while (true)
        {
            std::cout << "Child process " << getpid() << " is still alive" << std::endl;
            std::this_thread::sleep_for(5000ms);
        }
    }

    static void HandleSignal(int signal)
    {
        if (signal == SIGTERM || signal == SIGINT)
        {
            std::cout << "Child process " << getpid() << " received signal " << signal << ", exiting..." << std::endl;
            _exit(0);
        }
    }

    bool KillProcess(pid_t pid)
    {
        if (kill(pid, SIGTERM) < 0)
        {
            std::cerr << "Failed to send SIGTERM to process " << pid << ": " << strerror(errno) << std::endl;
            return false;
        }

        int status;
        if (waitpid(pid, &status, 0) < 0)
        {
            std::cerr << "Failed to wait for process " << pid << ": " << strerror(errno) << std::endl;
            return false;
        }
        return true;
    }
};

bool HandleCommand(ProcessManager &manager, const std::string &command)
{
    if (command == "spawn")
    {
        try
        {
            pid_t pid = manager.SpawnChild();
            if (pid > 0)
            {
                std::cout << "Spawned child process with PID " << pid << std::endl;
            }
        } catch (const std::exception &e)
        {
            std::cerr << "Error: " << e.what() << std::endl;
        }
    }
    else if (command == "kill")
    {
        manager.KillChild();
    }
    else if (command == "exit")
    {
        std::cout << "Exiting program" << std::endl;
        return false;
    }
    else
    {
        std::cerr << "Unknown command: " << command << std::endl;
    }
    return true;
}

int main()
{
    ProcessManager processManager;
    std::string command;

    while (true)
    {
        std::cout << "Enter the command: ";
        std::getline(std::cin, command);
        if (!HandleCommand(processManager, command))
        {
            break;
        }
    }
    kill 48542

    return 0;
}
