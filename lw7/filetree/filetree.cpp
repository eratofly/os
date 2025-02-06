#include <iostream>
#include <filesystem>
#include <vector>
#include <string>

namespace fs = std::filesystem;

// отступы
void printIndentation(int level)
{
    for (int i = 0; i < level; ++i)
    {
        std::cout << "|   ";
    }
}

//строим дерево рекурсивно
//1. убрать лишние палочки при переходе от директории к дериектории - вертикальная черта только для 1 уровня
//2. как получить путь к файлу внутри файла, если у нас путь к папке и имя файла
//3. как имея z получить x
//std::filesystem::path::parent_path
//Возвращает путь к родительскому элементу текущего пути (т.е. директорию, в которой находится файл).
//4. как скопировать файл череч fs
//В библиотеке std::filesystem для копирования файла используется функция std::filesystem::copy. Она поддерживает несколько режимов копирования, определяемых с помощью параметра std::filesystem::copy_options.
//std::filesystem::copy(source, destination, std::filesystem::copy_options::overwrite_existing);
//Режимы копирования (std::filesystem::copy_options):
//std::filesystem::copy_options::none: Копирует файл, но вызывает ошибку, если файл назначения уже существует.
//std::filesystem::copy_options::overwrite_existing: Перезаписывает файл назначения, если он уже существует.
//std::filesystem::copy_options::skip_existing: Пропускает копирование, если файл назначения существует.
//std::filesystem::copy_options::update_existing: Копирует только если исходный файл новее.
void PrintTree(const fs::path &path, int level, bool isLast)
{
    if (!fs::exists(path))
    {
        std::cerr << "Path does not exist: " << path << '\n';
        return;
    }

    if (level == 0)
    {
        std::cout << path.filename().string() << '\n';
    }
    // Отображение элемента, кроме корневого
    if (level > 0)
    {
        printIndentation(level - 1);

        if (fs::is_directory(path))
        {
            std::cout << (isLast ? "\---" : "+---") << "[D] ";
        } else
        {
            std::cout << "    " << "[F] ";
        }
        std::cout << path.filename().string() << '\n';
    }

    std::cout << '\n';

    if (fs::is_directory(path))
    {
        std::vector<fs::directory_entry> entries;
        for (const auto &entry: fs::directory_iterator(path))
        {
            entries.push_back(entry);
        }

        for (size_t i = 0; i < entries.size(); ++i)
        {
            PrintTree(entries[i].path(), level + 1, i == entries.size() - 1);
        }
    }
}

int main(int argc, char *argv[])
{
    fs::path directory = fs::current_path();

    if (argc > 1)
    {
        directory = argv[1];
    }

    PrintTree(directory, 0, true);

    fs::path directory1 = "/a/b/c/in.txt";
    fs::path file = "input.txt";

    return 0;
}
