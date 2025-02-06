#ifndef STUDENT_DB_H
#define STUDENT_DB_H

#include <string>
#include <vector>
#include <unordered_map>

struct Student
{
    unsigned int id;
    std::string name;
    std::string major;
    int year;
};

class StudentDB
{
private:
    std::unordered_map<unsigned int, Student> students;
public:
    bool AddStudent(const Student &student);

    bool RemoveStudent(unsigned int id);

    const Student *FindStudentById(unsigned int id) const;

    std::vector<Student> FindStudentsByName(const std::string &name) const;
};

#endif // STUDENT_DB_H
