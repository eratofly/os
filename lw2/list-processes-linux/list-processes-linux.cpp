#include <iostream>
#include <dirent.h>
#include <cstring>
#include <cctype>
#include <vector>
#include <fstream>
#include <sstream>

//вернуть пиды
std::vector<pid_t> GetAllPids()
{
    //сделать raii обёртку

    DIR *dir = opendir("/proc");
    struct dirent *entry;
    std::vector<pid_t> pids;

    //выбросить исключение
    if (!dir)
    {
        throw std::runtime_error("The /proc folder could not be opened");
    }

    //выяснить, что сделать с entry
    while ((entry = readdir(dir)) != nullptr)
    {
        if (entry->d_type == DT_DIR && isdigit(entry->d_name[0]))
        {
            pids.emplace_back(std::stoi(entry->d_name));
        }
    }

    closedir(dir);
    return pids;
}

std::vector<std::string> ParseString(const std::string &str, auto delimiter)
{
    std::istringstream iss(str);

    std::string token;
    std::vector<std::string> tokens;

    while (getline(iss, token, delimiter))
    {
        tokens.push_back(token);
    }

    return tokens;
}

int main()
{
    std::vector<pid_t> pids = GetAllPids();
    for (auto &pid: pids)
    {
        std::string processFile = "/proc/" + std::to_string(pid) + "/status";
        std::ifstream processStatusFile(processFile);
        if (!processStatusFile.is_open())
        {
            std::cout << "File " << processFile << " is not open";
            return EXIT_FAILURE;
        }
        std::string line, fileName, memory;
        while (std::getline(processStatusFile, line))
        {
            std::vector<std::string> parsedStr = ParseString(line, ':');
            //проверить что в есть как минимуми 2 элемента
            if (parsedStr.size() == 2)
            {
                if (parsedStr[0] == "Name")
                {
                    fileName = parsedStr[1];
                }
                if (parsedStr[0] == "VmSize")
                {
                    memory = parsedStr[1];
                }
                if (!fileName.empty() && !memory.empty())
                {
                    std::cout << "PID: " << pid << "   Name: " << fileName << "   Memory: " << memory << std::endl;
                    fileName = "";
                    memory = "";
                    break;
                }
            } else
            {
                break;
            }

        }
    }
}