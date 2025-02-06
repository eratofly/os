#include <QApplication>
#include <QWidget>
#include <QPainter>
#include <QMouseEvent>
#include <QMetaObject>
#include <QVector>
#include <boost/asio.hpp>
#include <thread>
#include <utility>
#include <vector>
#include <iostream>
#include <sstream>
#include <mutex>

using boost::asio::ip::tcp;

template<typename T>
using typeLine = std::vector<std::pair<T, T>>;

//Q_DECLARE_METATYPE — это макрос в Qt, который делает тип известным классу QMetaType
Q_DECLARE_METATYPE(typeLine<QPoint>)

class Whiteboard : public QWidget
{
Q_OBJECT

public:
    Whiteboard(bool isServer, std::string address, unsigned short port, QWidget *parent = nullptr)
            : QWidget(parent), m_isServer(isServer), m_address(std::move(address)), m_port(port), m_ioContext()
    {
        setFixedSize(800, 600);
        setWindowTitle(isServer ? "Server Whiteboard" : "Client Whiteboard");

        if (isServer)
        {
            m_networkThread = std::thread(&Whiteboard::StartServer, this);
        } else
        {
            m_networkThread = std::thread(&Whiteboard::StartClient, this);
            connect(this, &Whiteboard::UpdateLinesFromServer, this, &Whiteboard::HandleNewLines, Qt::QueuedConnection);
        }
    }

    ~Whiteboard() override
    {
        m_ioContext.stop();
        if (m_networkThread.joinable())
        {
            m_networkThread.join();
        }
    }

protected:
    void paintEvent(QPaintEvent *event) override
    {
        QPainter painter(this);
        painter.setPen(QPen(Qt::black, 4));
        for (const auto &line: m_lines)
        {
            painter.drawLine(line.first, line.second);
        }
    }

    void mousePressEvent(QMouseEvent *event) override
    {
        m_lastPoint = event->pos();
    }

    void mouseMoveEvent(QMouseEvent *event) override
    {
        if (m_isServer && (event->buttons() & Qt::LeftButton))
        {
            QPoint currentPoint = event->pos();
            m_lines.emplace_back(m_lastPoint, currentPoint);
            update();

            SendLineToClients(m_lastPoint, currentPoint);
            m_lastPoint = currentPoint;
        }
    }

signals:
    void UpdateLinesFromServer(const typeLine<QPoint> &newLines);

public slots:
    void HandleNewLines(const typeLine<QPoint> &newLines)
    {
        {
            std::lock_guard<std::mutex> lock(m_linesMutex);
            m_lines.insert(m_lines.end(), newLines.begin(), newLines.end());
        }
        update();
    }

private:
    bool m_isServer;
    std::string m_address;
    unsigned short m_port;

    QPoint m_lastPoint;
    typeLine<QPoint> m_lines;
    std::mutex m_linesMutex;

    boost::asio::io_context m_ioContext;
    std::thread m_networkThread;

    std::list<std::shared_ptr<tcp::socket>> m_clientSockets;
    std::mutex m_socketMutex;

    void StartServer()
    {
        try
        {
            tcp::acceptor acceptor(m_ioContext, tcp::endpoint(tcp::v4(), m_port));
            DoAccept(acceptor);
            m_ioContext.run();
        }
        catch (std::exception &e)
        {
            std::cerr << "Server error: " << e.what() << std::endl;
        }
    }

    void DoAccept(tcp::acceptor &acceptor)
    {
        auto socket = std::make_shared<tcp::socket>(m_ioContext);
        acceptor.async_accept(*socket, [this, socket, &acceptor](boost::system::error_code ec)
        {
            if (!ec)
            {
                {
                    std::lock_guard<std::mutex> lock(m_socketMutex);
                    m_clientSockets.push_back(socket);
                }
                DoAccept(acceptor);
            }
        });
    }

    void StartClient()
    {
        try
        {
            tcp::socket socket(m_ioContext);
            socket.connect(tcp::endpoint(boost::asio::ip::address::from_string(m_address), m_port));

            while (true)
            {
                boost::asio::streambuf buffer;
                boost::asio::read_until(socket, buffer, "\n");
                std::istream stream(&buffer);

                int x1, y1, x2, y2;
                if (stream >> x1 >> y1 >> x2 >> y2)
                {
                    QPoint start(x1, y1);
                    QPoint end(x2, y2);

                    typeLine<QPoint> newLines = {{start, end}};

                    //QMetaObject::invokeMethod — это мощный метод в библиотеке Qt, который используется для вызова слотов (или методов) объекта через его метаобъектную систему.
                    // Этот механизм полезен, когда вы хотите динамически вызывать метод или обеспечивать взаимодействие между потоками с использованием сигналов и слотов.
                    //Qt::QueuedConnection: Метод вызывается асинхронно в целевом потоке через очередь событий.
                    //Q_ARG — это макрос для передачи аргументов в системе метаобъектов Qt

                    //QMetaObject::invokeMethod вызывается, когда клиент получает от сервера данные о новых линиях
                    // Этот метод передаёт данные в основной поток приложения, где они обрабатываются и перерисовывается интерфейс

                    QMetaObject::invokeMethod(this, "UpdateLinesFromServer", Qt::QueuedConnection,
                                              Q_ARG(typeLine<QPoint>, newLines));
                }
            }
        }
        catch (std::exception &e)
        {
            std::cerr << "Client error: " << e.what() << std::endl;
        }
    }

    void SendLineToClients(const QPoint &start, const QPoint &end)
    {
        for (auto it = m_clientSockets.begin(); it != m_clientSockets.end();)
        {
            try
            {
                auto &socket = *it;

                if (socket && socket->is_open())
                {
                    std::ostringstream oss;
                    oss << start.x() << " " << start.y() << " " << end.x() << " " << end.y() << "\n";
                    boost::asio::write(*socket, boost::asio::buffer(oss.str()));

                    ++it;
                } else
                {
                    it = m_clientSockets.erase(it);
                }
            }
            catch (...)
            {
                it = m_clientSockets.erase(it);
            }
        }
    }
};

#include "draw.moc"

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);

    qRegisterMetaType<typeLine<QPoint>>("typeLine<QPoint>");

    if (argc == 2)
    {
        unsigned short port = std::stoi(argv[1]);
        Whiteboard whiteboard(true, "127.0.0.1", port);
        whiteboard.show();
        return QApplication::exec();
    } else if (argc == 3)
    {
        std::string address = argv[1];
        unsigned short port = std::stoi(argv[2]);
        Whiteboard whiteboard(false, address, port);
        whiteboard.show();
        return QApplication::exec();
    } else
    {
        std::cerr << "Usage:\n"
                  << "Server: whiteboard PORT\n"
                  << "Client: whiteboard ADDRESS PORT\n";
        return EXIT_FAILURE;
    }
}
