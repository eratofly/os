#include "StudentDBWrapper.h"  // Включаем наш файл с обёрткой

int main()
{
    StudentDBWrapper db;

    db.AddStudent(1, "Alice", "English", 2021);
    db.AddStudent(2, "Bob", "Math", 2022);

    std::string studentInfo = db.FindStudentById(1);
    if (!studentInfo.empty())
    {
        std::cout << "Found student: " << studentInfo << std::endl;
    }

    std::vector<std::string> students = db.FindStudentsByName("Alice");
    for (const auto &student: students)
    {
        std::cout << "Student: " << student << std::endl;
    }

    if (db.RemoveStudent(2))
    {
        std::cout << "Student with ID 2 removed." << std::endl;
    }

    return 0;
}
