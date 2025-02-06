#include "StudentDB.h"

bool StudentDB::AddStudent(const Student &student)
{
    //с помощью emplace мы добавляем студента с новым id, если id уже есть, то false, id нет, то true
    return students.emplace(student.id, student).second;
}

bool StudentDB::RemoveStudent(unsigned int id)
{
    return students.erase(id) > 0;
}

const Student *StudentDB::FindStudentById(unsigned int id) const
{
    auto it = students.find(id);
    return it != students.end() ? &it->second : nullptr;
}

std::vector<Student> StudentDB::FindStudentsByName(const std::string &name) const
{
    std::vector<Student> result;
    for (const auto &entry: students)
    {
        if (entry.second.name == name)
        {
            result.push_back(entry.second);
        }
    }
    return result;
}
