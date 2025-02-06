#include <iostream>
#include <unordered_set>
#include <vector>
#include <thread>
#include <mutex>
#include <shared_mutex>

//сделать его шаблонным
template <typename T, typename H = std::hash<T>, typename Comp = std::equal_to<T>>
class ThreadsafeSet
{
public:
    //добавить метод для чтения информации, что у нас лежит в массиве
    void Insert(uint64_t value)
    {
        //выяснить почему мы вызываем std::lock_guard
        std::lock_guard<std::shared_mutex> lock(m_mutex);
        //std::lock_guard — это специальный объект, который обеспечивает автоматическую блокировку и разблокировку мьютекса при его создании и уничтожении.
        //Он реализует паттерн RAII (Resource Acquisition Is Initialization), что означает, что блокировка происходит в момент создания объекта и автоматически снимается, когда объект выходит из области видимости (например, при выходе из функции).
        m_set.insert(value);
    }

    std::vector<T> Read() const
    {
        //std::shared_lock<std::shared_mutex> используется для только для чтения блокировки, что позволяет нескольким потокам одновременно читать данные, но блокирует доступ для изменения (записи) данных.
        std::shared_lock<std::shared_mutex> lock(m_mutex);
        return std::vector<T>(m_set.begin(), m_set.end());
    }

    void Print() const
    {
        std::shared_lock<std::shared_mutex> lock(m_mutex);
        std::cout << "Set elements: ";
        for (const auto& elem : m_set)
        {
            std::cout << elem << " ";
        }
        std::cout << std::endl;
    }

private:
    std::unordered_set<T, H, Comp> m_set;
    //чем выяснить shared_mutex и mutex
    //mutable - что такое и почему мы используем
    mutable std::shared_mutex m_mutex;

    //std::mutex — это объект, используемый для защиты данных от одновременного доступа из нескольких потоков в многозадачных приложениях.
    //Это объект, который блокирует доступ к ресурсу, чтобы только один поток мог его использовать в данный момент времени, предотвращая условия гонки (race conditions) и другие проблемы, связанные с параллельным доступом.

    //std::shared_mutex — это мьютекс, предназначенный для реализации разделенного доступа в многозадачных приложениях, когда несколько потоков могут одновременно читать данные, но только один поток может изменять их. Это особенно полезно, когда чтение данных происходит гораздо чаще, чем их изменение, так как позволяет значительно улучшить производительность при многозадачности.
};

//Ключевое слово mutable в C++ позволяет изменять члены класса даже в функциях, которые были объявлены как const.
// Это полезно для управления состоянием объекта, если есть члены, которые можно считать "логически неизменяемыми", но которые все же нужно модифицировать.

bool IsPrime(uint64_t num)
{
    if (num < 2) return false;
    for (uint64_t i = 2; i <= num / i; ++i)
    {
        if (num % i == 0)
            return false;
    }
    return true;
}

void FindPrimes(uint64_t start, uint64_t end, ThreadsafeSet<uint64_t> &primes)
{
    // Проверяем, что start не превышает end
    if (start > end) return;

    // Перебор чисел от start до end (включительно)
    //исправить бесконечный уикл
    for (uint64_t num = start; num <= end;)
    {
        if (IsPrime(num))
        {
            primes.Insert(num);
        }

        if (num++ == end)
        {
            break;
        }
    }
}

int main()
{
    uint64_t upperLimit = 100000;
    ThreadsafeSet<uint64_t> primes;
    std::chrono::steady_clock::time_point startTime;
    std::chrono::steady_clock::time_point endTime;
    std::chrono::duration<double> duration;

    startTime = std::chrono::steady_clock::now();

    FindPrimes(2, upperLimit, primes);

//    endTime = std::chrono::steady_clock::now();
//    duration = endTime - startTime;
//    std::cout << "Single-threaded algorithm: " << duration.count() << " seconds" << std::endl;

    startTime = std::chrono::steady_clock::now();
    int numThreads = 6;

    {
        std::vector<std::jthread> threads;
        for (int i = 0; i < numThreads; ++i) {
            threads.emplace_back(FindPrimes, upperLimit / numThreads * i + 2, upperLimit / numThreads * (i + 1), std::ref(primes));
        }
    }

    endTime = std::chrono::steady_clock::now();
    duration = endTime - startTime;
    std::cout << "Multi-threaded algorithm: " << duration.count() << " seconds" << std::endl;

    // Чтение данных
    std::vector<uint64_t> allPrimes = primes.Read();
    std::cout << "Number of primes found: " << allPrimes.size() << std::endl;

    // Вывод содержимого множества
//    primes.Print();

    return EXIT_SUCCESS;
}
//с помощью закона Амдала определить какая часть программы при 100 000 000 не распареллиливается (выполняется последовательно)