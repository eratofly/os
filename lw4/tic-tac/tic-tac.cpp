#include <iostream>
#include <thread>
#include <chrono>
#include <string>
#include <stop_token>
#include <sstream>
#include <iomanip>
#include <syncstream>

using namespace std::literals;
using Clock = std::chrono::system_clock;

std::string CurrentTimeToString()
{
    auto t = Clock::to_time_t(Clock::now());
    struct tm buf{};
    localtime_r(&t, &buf);
    std::ostringstream os;
    os << std::setw(2) << std::setfill('0') << buf.tm_hour << ":"
       << std::setw(2) << std::setfill('0') << buf.tm_min << ":"
       << std::setw(2) << std::setfill('0') << buf.tm_sec;
    return os.str();
}

void TicThread(std::stop_token stoken)
{
    while (!stoken.stop_requested())
    {
        {
            std::osyncstream sync(std::cout);
            sync << CurrentTimeToString() << " Tic" << std::endl;
        }
        std::this_thread::sleep_for(2s);
    }
    {
        std::osyncstream sync(std::cout);
        sync << "Tic thread completed" << std::endl;
    }
}

void TacThread(std::stop_token stoken)
{
    while (!stoken.stop_requested())
    {
        {
            std::osyncstream sync(std::cout);
            sync << CurrentTimeToString() << " Tac" << std::endl;
        }
        std::this_thread::sleep_for(3s);
    }
    {
        std::osyncstream sync(std::cout);
        sync << "Tac thread completed" << std::endl;
    }
}

int main()
{
    //не создавать stopsourse и не передавать токен
    std::jthread ticThread(TicThread);
    std::jthread tacThread(TacThread);

    std::cin.get();

    return 0;
}
