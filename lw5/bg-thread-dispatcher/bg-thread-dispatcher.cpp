#include <functional>
#include <queue>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <atomic>
#include <exception>
#include <iostream>
#include <syncstream>

class BgThreadDispatcher
{
public:
    using Task = std::function<void()>;

    BgThreadDispatcher(const BgThreadDispatcher &) = delete;

    BgThreadDispatcher &operator=(const BgThreadDispatcher &) = delete;

    BgThreadDispatcher() : m_stopFlag(false), m_thread(&BgThreadDispatcher::ThreadLoop, this) {}

    ~BgThreadDispatcher()
    {
        Stop();
    }

    void Dispatch(Task task)
    {
        std::lock_guard<std::mutex> lock(m_mutex);
        if (!m_stopFlag)
        {
            m_tasks.push(std::move(task));
            m_taskAddedOrStop.notify_one();
        }
    }

    void Wait()
    {
        std::unique_lock<std::mutex> lock(m_mutex);
        m_allTasksDone.wait(lock, [this]
        {
            return m_tasks.empty() && !m_taskRunning;
        });
    }

    void Stop()
    {
        std::lock_guard<std::mutex> lock(m_mutex);
        m_stopFlag = true;
        m_taskAddedOrStop.notify_one();
    }

private:
    std::queue<Task> m_tasks;
    std::mutex m_mutex;
    std::condition_variable m_allTasksDone;
    std::condition_variable m_taskAddedOrStop;
    //std::condition_variable m_allTasksDone;
    //    std::condition_variable m_taskAddedOrStop; разница
    std::jthread m_thread;
    std::atomic<bool> m_stopFlag;
    //std::atomic<bool> и bool в чём разница
    bool m_taskRunning = false;

    void ThreadLoop()
    {
        while (true)
        {
            Task task;
            //выяснить, что такое spurious wake ups
            {
                std::unique_lock<std::mutex> lock(m_mutex);
                if (m_tasks.empty())
                {
                    m_taskAddedOrStop.wait(lock, [this]
                    {
                        return m_stopFlag || !m_tasks.empty();
                    });
                }

                if (m_stopFlag && m_tasks.empty())
                {
                    break;
                }

                task = std::move(m_tasks.front());
                m_tasks.pop();
                m_taskRunning = true;
            }

            try
            {
                task();
            }
            catch (...)
            {
            }

            {
                std::lock_guard<std::mutex> lock(m_mutex);
                m_taskRunning = false;
                if (m_tasks.empty())
                {
                    m_allTasksDone.notify_all();
                }
            }
        }
    }
};

//int main()
// {
//    BgThreadDispatcher dispatcher;
//    dispatcher.Dispatch([] { std::cout << "1"; });
//    dispatcher.Dispatch([] { std::cout << "2"; });
//    std::cout << "!";
//    dispatcher.Wait(); // Дождется вывода "!12"
//}

//int main()
// {
//    BgThreadDispatcher dispatcher;
//    dispatcher.Dispatch([] { std::cout << "1"; });
//    std::this_thread::sleep_for(std::chrono::seconds(1));
//    dispatcher.Stop(); // Остановит выполнение задач
//    dispatcher.Dispatch([] { std::cout << "2"; });
//    dispatcher.Wait();
//    std::cout << "!"; // Выведет "1!"
//}

int main()
{
    using osync = std::osyncstream;
    BgThreadDispatcher dispatcher;
    dispatcher.Dispatch([] { osync(std::cout) << "1"; }); // выведет 1 в фоновом потоке
    dispatcher.Dispatch([] { osync(std::cout) << "2"; }); // Выведет 2 в фоновом потоке
    osync(std::cout) << "!"; // Выведет ! в основном потоке
    dispatcher.Wait(); // Дождётся печати 1 и 2
    dispatcher.Dispatch([] { osync(std::cout) << "3"; }); // Выведет 3 в фоновом потоке
    dispatcher.Wait(); // Дождётся вывода 3
}

//std::condition_variable в C++ — это примитив синхронизации, который позволяет одному потоку уведомлять другие об изменениях в общих данных, сигнализируя им ждать или продолжать работу
//std::atomic<bool> позволяет синхронизировать два потока.