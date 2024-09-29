#include <iostream>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <cstdlib>

int main() {
    pid_t child_pid;

    // Создание дочернего процесса
    child_pid = fork();

    if (child_pid == 0) {
        // Дочерний процесс
        exit(0);
    } else if (child_pid > 0) {
        // Родительский процесс
        int status;
        pid_t wait_pid;

        while (true) {
            std::cout << "Enter the PID of the child process: ";
            int pid;
            if (!(std::cin >> pid)) {
                std::cout << "Invalid input, try again." << std::endl;
                std::cin.clear();
//                std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
                continue;
            }

            wait_pid = waitpid(pid, &status, 0);
            if (wait_pid == pid) {
                std::cout << "Child process with PID " << pid << " has terminated." << std::endl;
                break;
            } else {
                std::cout << "Failed to wait for child process with PID " << pid << ", try again." << std::endl;
            }
        }
    } else {
        // Ошибка при создании дочернего процесса
        perror("fork");
        exit(1);
    }

    return 0;
}
