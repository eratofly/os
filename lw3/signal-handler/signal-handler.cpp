#include <iostream>
#include <fstream>
#include <filesystem>
#include <csignal>
#include <atomic>
#include <thread>
#include <chrono>
#include <utility>
#include <thread>

//какой сигнал можно передать процессу, чтобы он закончил свою работу независимо от свеого желания
//SIGKILL
std::atomic_flag exitRequested = ATOMIC_FLAG_INIT;

void SignalHandler(int s)
{
    std::cout << std::this_thread::get_id() << std::endl;
    if (s == SIGINT || s == SIGTERM || s == SIGABRT)
    {
        std::cout << "Process " << getpid() << " has received signal #" << s << std::endl;
        exitRequested.test_and_set();
    }
}

class TempFile
{
public:
    explicit TempFile(std::string filename) : m_filename(std::move(filename))
    {
        m_ofstream.open(m_filename);
        if (!m_ofstream.is_open())
        {
            throw std::runtime_error("Failed to create file");
        }
    }

    ~TempFile()
    {
        try
        {
            if (m_ofstream.is_open())
            {
                m_ofstream.close();
            }
            if (std::filesystem::exists(m_filename))
            {
                std::filesystem::remove(m_filename);
                std::cout << "File " << m_filename << " has been removed." << std::endl;
            }
        } catch (const std::filesystem::filesystem_error &e)
        {
            std::cerr << "Can not remove file: " << e.what() << std::endl;
        }
    }

    void WriteNumber(int num)
    {
        if (m_ofstream.is_open())
        {
            m_ofstream << num << std::endl;
        }
    }

private:
    std::string m_filename;
    std::ofstream m_ofstream;
};

int main()
{
    std::cout << std::this_thread::get_id() << std::endl;
    signal(SIGINT, SignalHandler);
    signal(SIGTERM, SignalHandler);
    signal(SIGABRT, SignalHandler);

    try
    {
        TempFile tempFile("temp.txt");

        for (int i = 0; i <= 100; ++i)
        {
            if (exitRequested.test())
            {
                std::cout << "Termination signal received" << std::endl;
                break;
            }
            std::cout << i << std::endl;
            tempFile.WriteNumber(i);
            std::this_thread::sleep_for(std::chrono::seconds(1));
        }
        std::cout << "Done" << std::endl;
    } catch (const std::exception &e)
    {
        std::cerr << "Error: " << e.what() << "" << std::endl;
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}
