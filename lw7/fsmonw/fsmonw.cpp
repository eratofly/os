#include <windows.h>
#include <iostream>
#include <string>
#include <vector>

// Функция для отображения событий
void printEvent(const std::string& filename, DWORD action)
{
    std::cout << "File: " << filename << " - Event: ";

    switch (action) {
        case FILE_ACTION_ADDED:
            std::cout << "Created";
            break;
        case FILE_ACTION_REMOVED:
            std::cout << "Deleted";
            break;
        case FILE_ACTION_MODIFIED:
            std::cout << "Modified";
            break;
        case FILE_ACTION_RENAMED_OLD_NAME:
            std::cout << "Renamed (old name)";
            break;
        case FILE_ACTION_RENAMED_NEW_NAME:
            std::cout << "Renamed (new name)";
            break;
        default:
            std::cout << "Unknown event";
            break;
    }
    std::cout << '\n';
}

int main(int argc, char* argv[])
{
    std::cout << "ffff";
    if (argc != 2) {
        std::cout << "Usage: fsmonw PATH\n";
        return 1;
    }

    std::string directory = argv[1];
    std::cout << directory;

    // Открытие каталога для мониторинга
    HANDLE hDir = CreateFile(
            directory.c_str(),
            FILE_LIST_DIRECTORY,
            FILE_SHARE_READ | FILE_SHARE_WRITE | FILE_SHARE_DELETE,
            nullptr,
            OPEN_EXISTING,
            FILE_FLAG_BACKUP_SEMANTICS,
            nullptr
    );

    if (hDir == INVALID_HANDLE_VALUE) {
        std::cerr << "Error: Unable to open directory. Code: " << GetLastError() << '\n';
        return 1;
    }

    std::cout << "Monitoring directory: " << directory << '\n';

    constexpr DWORD bufferSize = 64 * 1024; // Буфер для уведомлений
    char buffer[bufferSize];
    DWORD bytesReturned;

    std::cout << hDir;

    while (true) {
        if (!ReadDirectoryChangesW(
                hDir,
                buffer,
                bufferSize,
                TRUE, // Рекурсивное наблюдение
                FILE_NOTIFY_CHANGE_FILE_NAME |
                FILE_NOTIFY_CHANGE_DIR_NAME |
                FILE_NOTIFY_CHANGE_SIZE |
                FILE_NOTIFY_CHANGE_ATTRIBUTES,
                &bytesReturned,
                nullptr,
                nullptr
        )) {
            std::cerr << "Error: ReadDirectoryChangesW failed. Code: " << GetLastError() << '\n';
            break;
        }

//        char* ptr = buffer;
//        while (ptr < buffer + bytesReturned) {
//            auto* fni = reinterpret_cast<FILE_NOTIFY_INFORMATION*>(ptr);
//
//            // Преобразование имени файла из WCHAR в std::string
//            std::wstring wFilename(fni->FileName, fni->FileNameLength / sizeof(WCHAR));
//            std::string filename(wFilename.begin(), wFilename.end());
//
//            printEvent(filename, fni->Action);
//
//            if (fni->NextEntryOffset == 0) {
//                break;
//            }
//            ptr += fni->NextEntryOffset;
//        }
    }

    CloseHandle(hDir);
//    return 0;
}
