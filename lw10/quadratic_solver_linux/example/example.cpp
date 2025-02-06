#include <iostream>
#include <vector>
#include <dlfcn.h>

int main() {
    double a, b, c;

    std::cout << "Enter coefficients a, b, and c: ";
    std::cin >> a >> b >> c;

    void* handle = dlopen("../quadratic_solver_lib/libQuadraticSolverLib.so", RTLD_LAZY);
    if (!handle) {
        std::cerr << "Error loading library: " << dlerror() << std::endl;
        return 1;
    }

    typedef std::vector<double> (*SolveQuadraticFunc)(double, double, double);
    auto SolveQuadratic = (SolveQuadraticFunc)dlsym(handle, "SolveQuadratic");

    char* error = dlerror();
    if (error != nullptr)
    {
        std::cerr << "Error finding function: " << error << std::endl;
        dlclose(handle);
        return 1;
    }

    try
    {
        std::vector<double> roots = SolveQuadratic(a, b, c);

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

    dlclose(handle);
    return 0;
}
