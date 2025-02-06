#include <iostream>
#include "../quadratic_solver_lib/QuadraticSolverLib.h"

int main()
{
    double a, b, c;

    std::cout << "Enter coefficients a, b, and c: ";
    std::cin >> a >> b >> c;

    try
    {
        auto roots = SolveQuadratic(a, b, c);

        if (roots.empty())
        {
            std::cout << "No real roots.\n";
        } else
        {
            std::cout << "Roots: ";
            for (double root : roots)
            {
                std::cout << root << " ";
            }
            std::cout << "\n";
        }
    }
    catch (const std::exception& e)
    {
        std::cerr << "Error: " << e.what() << "\n";
    }

    return 0;
}
