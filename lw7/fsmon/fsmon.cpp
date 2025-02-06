#include <iostream>
#include <string>
#include <sys/inotify.h>
#include <unistd.h>
#include <climits>
#include <vector>
#include <filesystem>

namespace fs = std::filesystem;

// Функция для интерпретации событий inotify
void HandleEvent(const inotify_event* event)
{
    std::string eventName = (event->len > 0) ? event->name : "<unknown>";

    std::cout << "File: " << eventName << " Event: ";

    if (event->mask & IN_CREATE) std::cout << "Created ";
    if (event->mask & IN_DELETE) std::cout << "Deleted ";
    if (event->mask & IN_MODIFY) std::cout << "Modified ";
    if (event->mask & IN_ATTRIB) std::cout << "Attributes changed ";
    if (event->mask & IN_DELETE_SELF) std::cout << "Directory deleted ";
    if (event->mask & IN_MOVE_SELF) std::cout << "Directory moved ";
    if (event->mask & IN_MOVED_FROM) std::cout << "Moved from ";
    if (event->mask & IN_MOVED_TO) std::cout << "Moved to ";

    std::cout << '\n';
}

int main(int argc, char* argv[])
{
    if (argc != 2) {
        std::cerr << "Usage: fsmon PATH" << std::endl;
        return 1;
    }

    fs::path directory = argv[1];

    if (!fs::exists(directory) || !fs::is_directory(directory)) {
        std::cerr << "Error: Specified path is not a valid directory.\n";
        return 1;
    }

    //использовать RAII обёртку

    int inotifyFd = inotify_init(); // Создаем inotify-инстанс
    if (inotifyFd == -1) {
        perror("inotify_init");
        return 1;
    }
    // чем отличается IN_DELETE_SELF от IN_DELETE
    int watchDescriptor = inotify_add_watch(inotifyFd, directory.c_str(),
                                            IN_CREATE | IN_DELETE | IN_MODIFY | IN_ATTRIB |
                                            IN_DELETE_SELF | IN_MOVE_SELF | IN_MOVED_FROM | IN_MOVED_TO);
    if (watchDescriptor == -1) {
        perror("inotify_add_watch");
        close(inotifyFd);
        return 1;
    }

    std::cout << "Monitoring directory: " << directory << '\n';

    constexpr size_t bufferSize = 10 * (sizeof(inotify_event) + NAME_MAX + 1);
    std::cout << bufferSize << std::endl;
    std::vector<char> buffer(bufferSize);

    while (true) {
        ssize_t length = read(inotifyFd, buffer.data(), buffer.size());
        if (length == -1) {
            perror("read");
            break;
        }

        for (size_t i = 0; i < static_cast<size_t>(length); ) {
            const auto* event = reinterpret_cast<const inotify_event*>(&buffer[i]);
            HandleEvent(event);
            i += sizeof(inotify_event) + event->len;
        }
    }

     //очистка ресурсов
    inotify_rm_watch(inotifyFd, watchDescriptor);
    close(inotifyFd);
    return 0;
}
