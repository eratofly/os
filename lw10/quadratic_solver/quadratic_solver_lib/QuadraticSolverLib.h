#ifndef QUADRATIC_SOLVER_H
#define QUADRATIC_SOLVER_H

//добавить условия dllimport
#ifdef _WIN32
#define EXPORT __declspec(dllexport)
#else
#define EXPORT
#endif

#include <vector>
//вернуть структуру
extern "C" std::vector<double> SolveQuadratic(double a, double b, double c);

#endif
