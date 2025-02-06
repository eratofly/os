#include <iostream>
#include <vector>
#include <sstream>
#include <thread>
#include <cstring>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>

constexpr const int BUFFER_SIZE = 1024;

//КАКИМ ОБРАЗОМ ОТВЕТ СЕРВЕРА ДОХОДИТ ДО КЛИЕНТА?

template<typename Func, typename... Args>
auto CheckFunctionCall(Func func, Args... args)
{
    //invoke_result_t — это шаблон в C++, который позволяет определить тип результата во время компиляции.
    static_assert(std::is_invocable_v<Func, Args...>, "Provided function is not invocable with the given arguments.");
    using ResultType = std::invoke_result_t<Func, Args...>;//разобраться

    //
    auto result = func(args...);

    //constexpr??
    if constexpr (std::is_integral_v<ResultType>)
    {
        if (result < 0)
        {
            int err = errno;
            std::ostringstream errorMessage;
            errorMessage << "Function '" << __func__ << "' failed with error code: " << err << " (" << strerror(err) << ")";
            throw std::runtime_error(errorMessage.str());
        }
    }

    return result;
}

class DescriptorHandler
{
public:
    explicit DescriptorHandler(int m_fd) : m_fd(m_fd)
    {
        if (m_fd < 0)
        {
            throw std::runtime_error("Failed to create file descriptor");
        }
    }

    ~DescriptorHandler()
    {
        Close();
    }

    DescriptorHandler(const DescriptorHandler&) = delete;
    DescriptorHandler& operator=(const DescriptorHandler&) = delete;

    DescriptorHandler(DescriptorHandler&& other) noexcept: m_fd(other.m_fd)
    {
        other.m_fd = -1;
    }

    //noexcept в C++ — это спецификатор времени компиляции, который говорит компилятору о том, что функция не будет выбрасывать исключения. 1
    DescriptorHandler& operator=(DescriptorHandler&& other) noexcept
    {
        if (this != &other)
        {
            Close();
            m_fd = std::exchange(other.m_fd, -1);
        }
        return *this;
    }

    void Close()
    {
        if (m_fd >= 0)
        {
            close(m_fd);
        }
    }

    [[nodiscard]] int get() const
    {
        return m_fd;
    }

private:
    int m_fd;
};

//каким образом способен обрабатывать несколько подклбчений в одном потоке

std::vector<int> GetNumbersFromStream(std::istringstream& input)
{
    std::vector<int> numbers;
    int num;
    while (input >> num)
    {
        numbers.push_back(num);
    }

    return numbers;
}
//принимать по констаной ссылке
std::string HandleCommandAdd(std::vector<int>& numbers)
{
    std::string commandResult;

    if (numbers.empty())
    {
        commandResult += "Error: No numbers provided";
    }
    else
    {
        int sum = 0;
        for (int n : numbers)
        {
            sum += n;
        }
        commandResult += std::to_string(sum);
    }

    return commandResult;
}

std::string HandleCommandSub(std::vector<int>& numbers)
{
    std::string commandResult;

    if (numbers.empty())
    {
        commandResult += "Error: No numbers provided";
    }
    else
    {
        int result = 0;
        for (int number : numbers)
        {
            result -= number;
        }
        commandResult += std::to_string(result);
    }

    return commandResult;
}

std::string HandleCommands(std::vector<char>& buffer, long bytesReceived)
{
    std::istringstream input(std::string(buffer.begin(), buffer.begin() + bytesReceived));
    std::string command;
    input >> command;

    std::vector<int> numbers = GetNumbersFromStream(input);
    std::string result;
    if (command == "+")
    {
        result += HandleCommandAdd(numbers);
    }
    else if (command == "-")
    {
        result += HandleCommandSub(numbers);
    }
    else
    {
        result += "Error: Unknown command.";
    }

    return result;
}

void HandleClient(DescriptorHandler clientSocket)
{
    try
    {
        std::vector<char> buffer;
        buffer.reserve(BUFFER_SIZE);
        while (true)
        {
            buffer.clear();
            //BUFFER_SIZE - надо расширить
            buffer.resize(BUFFER_SIZE);
            auto bytesReceived = recv(clientSocket.get(), buffer.data(), buffer.size() - 1, 0);
            if (bytesReceived <= 0)
            {
                std::cout << "Client disconnected" << std::endl;
                break;
            }

            std::string response = HandleCommands(buffer, bytesReceived);
            send(clientSocket.get(), response.c_str(), response.size(), 0);
        }
    }
    catch (const std::exception& e)
    {
        std::cerr << "Error handling client: " << e.what() << std::endl;
    }
}

void InitializeServer(DescriptorHandler& serverSocket, int port)
{
    int opt = 1;
    CheckFunctionCall(setsockopt, serverSocket.get(), SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

    sockaddr_in serverAddr{};
    serverAddr.sin_family = AF_INET;
    //AF_INET — это семейство адресов, которое используется для обозначения типа адресов, с которыми может взаимодействовать сокет (в данном случае, адреса Internet Protocol v4). 1
    //
    //Константа соответствует Internet-домену, сокеты, размещённые в этом домене, могут использоваться для работы в любой IP-сети.
    serverAddr.sin_addr.s_addr = INADDR_ANY;
    //INADDR_ANY — это специальный IP-адрес, который используется, когда не нужно привязывать сокет к какому-либо конкретному IP. 1
    //
    //Он позволяет серверу получать пакеты, направленные через любой из интерфейсов. 1
    serverAddr.sin_port = htons(port);

    CheckFunctionCall(bind, serverSocket.get(), (struct sockaddr*) &serverAddr, sizeof(serverAddr));
    CheckFunctionCall(listen, serverSocket.get(), 5);
}

void RunServer(int port)
{
    //SOCK_STREAM — это тип сокета, который обеспечивает передачу потока данных с предварительной установкой соединения.
    DescriptorHandler serverSocket(socket(AF_INET, SOCK_STREAM, 0));
    InitializeServer(serverSocket, port);

    std::cout << "Server is listening on port " << port << std::endl;
    while (true)
    {
        sockaddr_in clientAddr{};
        socklen_t clientLen = sizeof(clientAddr);
        int clientFd = CheckFunctionCall(accept, serverSocket.get(), (struct sockaddr*) &clientAddr, &clientLen);
        if (clientFd < 0)
        {
            continue;
        }

        std::cout << "Client connected" << std::endl;
        std::thread(HandleClient, DescriptorHandler(clientFd)).detach();
        //В данном коде вызов detach() используется для того, чтобы выделить поток (thread) для обработки клиента и позволить ему работать независимо от основного потока программы
        //если поток будет не отсоединён, то при разркшении объекта программа завершится аварийно
        //Сервер будет ждать завершения каждого потока, что заблокирует основной поток программы и это нужно для подключения новых клиентов
    }
}

void InitializeClient(DescriptorHandler& clientSocket, const std::string& address, int port)
{
    sockaddr_in serverAddr{};
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(port);
    //Функция htons в C++ осуществляет перевод целого короткого числа из порядка байт, принятого на компьютере, в сетевой порядок байт.
    //Сетевой порядок байт (network byte order) — это стандартный порядок от старшего к младшему (big-endian), который используется в протоколах TCP/IP, в заголовках пакетов данных и во многих протоколах более высокого уровня, разработанных для использования поверх TCP/IP
    CheckFunctionCall(inet_pton, AF_INET, address.c_str(), &serverAddr.sin_addr);
    //что такое inet_pton
    CheckFunctionCall(connect, clientSocket.get(), (struct sockaddr*) &serverAddr, sizeof(serverAddr));
}

void RunClient(const std::string& address, int port)
{
    DescriptorHandler clientSocket(socket(AF_INET, SOCK_STREAM, 0));
    InitializeClient(clientSocket, address, port);

    std::cout << "Connected to server" << std::endl;
    std::string input;
    while (true)
    {
        std::cout << "> ";
        std::getline(std::cin, input);

        if (input.empty())
        {
            continue;
        }
        if (input == "exit")
        {
            std::cout << "Disconnecting from server..." << std::endl;
            break;
        }

        send(clientSocket.get(), input.c_str(), input.size(), 0);

        std::vector<char> buffer(1024);
        auto bytesReceived = recv(clientSocket.get(), buffer.data(), buffer.size(), 0);

        //Функция recv служит для чтения данных из сокета. 5


        if (bytesReceived <= 0)
        {
            std::cout << "Server disconnected" << std::endl;
            break;
        }

        std::cout << std::string(buffer.begin(), buffer.begin() + bytesReceived) << std::endl;
    }
}

int main(int argc, char* argv[])
{
    try
    {
        if (argc == 2)
        {
            int port = std::stoi(argv[1]);
            RunServer(port);
        }
        else if (argc == 3)
        {
            std::string address = argv[1];
            int port = std::stoi(argv[2]);
            RunClient(address, port);
        }
        else
        {
            std::cerr << "Usage: " << std::endl
                      << "  * Server: " << argv[0] << " <port>" << std::endl
                      << "  * Client: " << argv[0] << " <address> <port>" << std::endl;
            return EXIT_FAILURE;
        }
    }
    catch (const std::exception& e)
    {
        std::cerr << "Error: " << e.what() << std::endl;
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}

//Файловый дескриптор — это неотрицательное число, которое является идентификатором потока ввода-вывода. Дескриптор может быть связан с файлом, каталогом, сокетом. 1
//Когда процесс открывает файл или устройство, операционная система создаёт дескриптор файла для отслеживания открытого ресурса. Этот дескриптор служит ссылкой, через которую процесс может читать, записывать или управлять этим ресурсом. 2
//
//По умолчанию Unix-оболочки связывают файловый дескриптор 0 с потоком стандартного ввода (клавиатура), файловый дескриптор 1 — с потоком стандартного вывода (терминал), и файловый дескриптор 2 — со стандартным выводом ошибок (диагностические и отладочные сообщения, информация об ошибках). 3

//Программа обрабатывает несколько подключений в одном потоке с использованием функции select или ее аналогов. Однако, в предоставленном коде многопоточность достигается за счет выделения отдельного потока (std::thread) на каждое подключение клиента. Каждый новый клиент обслуживается своим потоком, что позволяет одновременно обрабатывать несколько клиентов.
//
//Вот более детальное объяснение:
//
//Как обрабатываются подключения
//Основной поток сервера:
//Серверный сокет прослушивает входящие соединения (listen).
//Принимаются подключения клиентов с помощью accept. При успешном подключении создается новый файловый дескриптор клиента (clientFd).
//Создание потока для клиента:
//
//Для каждого нового подключения создается поток (std::thread) с функцией HandleClient. Этот поток обрабатывает взаимодействие с клиентом.
//Параллельная обработка:
//
//Каждый поток выполняет чтение данных от клиента (recv) и отправку ответа (send) независимо от других потоков.
//Таким образом, сервер может обрабатывать множество клиентов одновременно.