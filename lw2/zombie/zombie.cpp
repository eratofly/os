#include <iostream>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <cstdlib>

int main()
{
    pid_t childPid = fork();

    if (childPid == 0)
    {
        return EXIT_SUCCESS;
    } else if (childPid > 0)
    {
        int status;
        pid_t waitPid;

        while (true)
        {
            std::cout << "Enter the PID of the child process: ";
            int pid;
            if (!(std::cin >> pid))
            {
                std::cout << "Invalid input, try again." << std::endl;
            }

            waitPid = waitpid(pid, &status, 0);
            if (waitPid == pid)
            {
                std::cout << "Child process with PID " << pid << " has terminated." << std::endl;
                break;
            } else
            {
                std::cout << "Failed to wait for child process with PID " << pid << ", try again." << std::endl;
            }
        }
    } else
    {
        std::cerr << "Fork process is invalid";
    }

    return 0;
}
