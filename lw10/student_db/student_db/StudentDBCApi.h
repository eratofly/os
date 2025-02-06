#ifndef STUDENT_DB_C_API_H
#define STUDENT_DB_C_API_H

#ifdef _WIN32
#define EXPORT __declspec(dllexport)
#else
#define EXPORT
#endif

#ifdef __cplusplus
extern "C" {
#endif

typedef struct StudentDBAPI StudentDBAPI;

EXPORT StudentDBAPI *CreateStudentDb();

EXPORT void DestroyStudentDb(StudentDBAPI *db);

EXPORT int AddStudent(StudentDBAPI *db, unsigned int id, const char *name, const char *major, int year);

EXPORT int RemoveStudent(StudentDBAPI *db, unsigned int id);

EXPORT const char *FindStudentById(StudentDBAPI *db, unsigned int id);

typedef void *StudentIterator;

EXPORT StudentIterator FindStudentsByName(StudentDBAPI *db, const char *name);
EXPORT const char *IteratorNext(StudentIterator it);
EXPORT void DestroyIterator(StudentIterator it);

EXPORT void FreeString(const char *str);

#ifdef __cplusplus
}
#endif

#endif // STUDENT_DB_C_API_H
