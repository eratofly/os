#include "QuadraticSolverLib.h"
#include <cmath>
#include <stdexcept>

std::vector<double> SolveQuadratic(double a, double b, double c) {
    std::vector<double> roots{};

    if (a == 0) {
        roots.push_back(-c/b);
        return roots;
    }

    double discriminant = b * b - 4 * a * c;

    if (discriminant > 0)
    {
        roots.push_back((-b + std::sqrt(discriminant)) / (2 * a));
        roots.push_back((-b - std::sqrt(discriminant)) / (2 * a));
    }
    else if (discriminant == 0)
    {
        roots.push_back(-b / (2 * a));
    }

    return roots;
}