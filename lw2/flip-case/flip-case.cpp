#include <iostream>
#include <fstream>
#include <cctype>
#include <unistd.h>
#include <sys/wait.h>

void FlipCase(const std::string &inputFile, const std::string &outputFile)
{
    std::ifstream input(inputFile);
    std::ofstream output(outputFile);

    if (!input.is_open() || !output.is_open())
    {
        throw std::runtime_error("Can't open file or files.");
    }

    char c;
    while (input.get(c))
    {
        if (isalpha(c))
        {
            c = isupper(c) ? tolower(c) : toupper(c);
        }
        output.put(c);
    }

    input.close();
    output.close();
}

int main(int argc, char *argv[])
{
    if (argc < 2)
    {
        std::cerr << "Usage: " << argv[0] << " <file1> [file2] [file3] ..." << std::endl;
        return 1;
    }

    for (int i = 1; i < argc; ++i)
    {
        pid_t childPid = fork();

        if (childPid == 0)
        {
            std::string inputFile = argv[i];
            std::string outputFile = inputFile + ".out";
            std::cout << "Process " << getpid() << " is processing " << inputFile << std::endl;
            FlipCase(inputFile, outputFile);
            std::cout << "Process " << getpid() << " has finished writing to " << outputFile << std::endl;
            break;
        }
        else if (childPid > 0)
        {
            //доработать, чтобы все процессы сразу создались, а потом только дождаться. запомнить айди дождаться, или в waipid передать -1
            int status;
            //while ()
            int status;
            waitpid(childPid, &status, 0);
            std::cout << "Child process " << childPid << " is over" << std::endl;
        }
        else
        {
            std::cerr << "Failed to create child process." << std::endl;
            return 1;
        }
    }

    return 0;
}
