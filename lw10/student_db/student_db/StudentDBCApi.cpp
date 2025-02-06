#include "StudentDBCApi.h"
#include "StudentDB.h"
#include <string>
#include <cstring>
#include <map>
#include <iterator>

struct StudentDBAPI
{
    StudentDB db;
};

struct StudentIteratorImpl
{
    std::vector<Student> students;
    size_t index = 0;
};

extern "C" {

EXPORT StudentDBAPI *CreateStudentDb()
{
    return new StudentDBAPI();
}

EXPORT void DestroyStudentDb(StudentDBAPI *db)
{
    delete db;
}

EXPORT int AddStudent(StudentDBAPI *db, unsigned int id, const char *name, const char *major, int year)
{
    return db->db.AddStudent({id, name, major, year}) ? 0 : -1;
}

EXPORT int RemoveStudent(StudentDBAPI *db, unsigned int id)
{
    return db->db.RemoveStudent(id) ? 0 : -1;
}

EXPORT const char *FindStudentById(StudentDBAPI *db, unsigned int id)
{
    const Student *student = db->db.FindStudentById(id);
    if (!student) return nullptr;

    std::string studentInfo = "ID:" + std::to_string(student->id) +
                              ", Name:" + student->name +
                              ", Major:" + student->major +
                              ", Year:" + std::to_string(student->year);

    char *result = (char *) malloc(studentInfo.size() + 1);
    if (result)
    {
        std::strcpy(result, studentInfo.c_str());
    }
    return result;
}

EXPORT StudentIterator FindStudentsByName(StudentDBAPI *db, const char *name)
{
    auto *it = new StudentIteratorImpl();
    it->students = db->db.FindStudentsByName(name);
    return it;
}

EXPORT const char *IteratorNext(StudentIterator it)
{
    auto *impl = static_cast<StudentIteratorImpl *>(it);
    if (impl->index >= impl->students.size()) return nullptr;

    const Student &student = impl->students[impl->index++];
    std::string studentInfo = "ID:" + std::to_string(student.id) +
                              ", Name:" + student.name +
                              ", Major:" + student.major +
                              ", Year:" + std::to_string(student.year);

    char *result = (char *) malloc(studentInfo.size() + 1);
    if (result)
    {
        std::strcpy(result, studentInfo.c_str());
    }
    return result;
}

EXPORT void DestroyIterator(StudentIterator it)
{
    delete static_cast<StudentIteratorImpl *>(it);
}

EXPORT void FreeString(const char *str)
{
    if (str)
    {
        free((void *) str);
    }
}

}
