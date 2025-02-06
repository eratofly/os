#include "StudentDBWrapper.h"

StudentDBWrapper::StudentDBWrapper() {
    db = CreateStudentDb();
}

StudentDBWrapper::~StudentDBWrapper() {
    DestroyStudentDb(db);
}

bool StudentDBWrapper::AddStudent(unsigned int id, const std::string &name, const std::string &major, int year) {
    return ::AddStudent(db, id, name.c_str(), major.c_str(), year) == 0;
}

bool StudentDBWrapper::RemoveStudent(unsigned int id) {
    return ::RemoveStudent(db, id) == 0;
}

std::string StudentDBWrapper::FindStudentById(unsigned int id) {
    const char *studentInfo = ::FindStudentById(db, id);
    std::string result;
    if (studentInfo) {
        result = studentInfo;
        FreeString(studentInfo);
    }
    return result;
}

std::vector<std::string> StudentDBWrapper::FindStudentsByName(const std::string &name) {
    std::vector<std::string> result;
    StudentIterator it = ::FindStudentsByName(db, name.c_str());
    const char *studentStr;
    while ((studentStr = IteratorNext(it)) != nullptr) {
        result.push_back(studentStr);
        FreeString(studentStr);
    }
    DestroyIterator(it);
    return result;
}

