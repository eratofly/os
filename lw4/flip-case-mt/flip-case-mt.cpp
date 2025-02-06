#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <thread>
#include <algorithm>

char ToLower(char ch)
{
    return static_cast<char>(std::tolower(static_cast<unsigned char>(ch)));
}

char ToUpper(char ch)
{
    return static_cast<char>(std::toupper(static_cast<unsigned char>(ch)));
}

void FlipCase(const std::string &inputFile, const std::string &outputFile)
{
    //выяснить, что произошло, когда мы добавили sleep?
    sleep(1);
    std::ifstream inFile(inputFile);
    if (!inFile)
    {
        std::cerr << "Ошибка открытия файла: " << inputFile << std::endl;
        return;
    }

    std::ofstream outFile(outputFile);
    if (!outFile)
    {
        std::cerr << "Ошибка открытия файла: " << outputFile << std::endl;
        return;
    }

    char ch;
    while (inFile.get(ch))
    {
        if (std::isupper(ch))
        {
            //вынести в функцию ту страшную штуку
            outFile.put(ToLower(ch));
        }
        else if (std::islower(ch))
        {
            outFile.put(ToUpper(ch));
        }
        else
        {
            outFile.put(ch);
        }
    }
}

int main(int argc, char *argv[])
{
    if (argc < 2)
    {
        std::cerr << "Использование: " << argv[0] << " <имя_входного_файла1> <имя_входного_файла2> ..." << std::endl;
        return EXIT_FAILURE;
    }

    //чем отличаются threads и jthreads
    //join() -- что делает?
    //что делает detach()?
    // except that jthread automatically rejoins on destruction, and can be cancelled/stopped in certain situations.
    //При выходе из области видимости std::jthread автоматически вызывает join(), что предотвращает утечки ресурсов и неопределённое поведение.
    std::vector<std::jthread> threads;

    for (int i = 1; i < argc; ++i)
    {
        std::string inputFile = argv[i];
        std::string outputFile = inputFile + ".out";
//выяснить как в функцию потока передать параметр по ссылке
//std::ref создает объект, который ведет себя аналогично ссылке
//Конструктор std::thread копирует переданные значения, не преобразуя их в ожидаемый тип аргумента. Поэтому нам нужно заключать аргументы, которые действительно должны быть ссылками, в std::ref.
        threads.emplace_back(FlipCase, std::ref(inputFile), std::ref(outputFile));
    }

    //что произойдет с потоком, если return 0
    //у нас в jthread Поддерживает механизм прерывания потоков с помощью std::stop_token. Это позволяет вам безопасно остановить выполнение потока, передав токен в функцию, выполняемую в потоке.
    return EXIT_SUCCESS;
}
