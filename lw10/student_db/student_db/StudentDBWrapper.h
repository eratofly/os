#include "StudentDBCApi.h"
#include <iostream>
#include <vector>
#include <string>

#ifndef STUDENT_DATABASE_H
#define STUDENT_DATABASE_H

#include "StudentDBCApi.h"
#include <string>
#include <vector>

class StudentDBWrapper
{
private:
    StudentDBAPI *db;

public:
    StudentDBWrapper();

    ~StudentDBWrapper();

    bool AddStudent(unsigned int id, const std::string &name, const std::string &major, int year);

    bool RemoveStudent(unsigned int id);

    std::string FindStudentById(unsigned int id);

    std::vector<std::string> FindStudentsByName(const std::string &name);
};

#endif // STUDENT_DATABASE_H