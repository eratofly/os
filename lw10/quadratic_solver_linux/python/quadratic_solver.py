import ctypes
import os

# Определение пути к библиотеке
lib_path = os.path.abspath("./libquadratic_solver.so")

# Загрузка библиотеки
solver_lib = ctypes.CDLL(lib_path)

# Определение аргументов и возвращаемого значения функции
solver_lib.solve_quadratic.argtypes = [ctypes.c_double, ctypes.c_double, ctypes.c_double]
solver_lib.solve_quadratic.restype = ctypes.POINTER(ctypes.c_double)

def solve_quadratic(a, b, c):
    # Вызов функции из библиотеки
    solver_lib.solve_quadratic.restype = ctypes.POINTER(ctypes.c_double)
    result = solver_lib.solve_quadratic(a, b, c)
    return result[:2]

if __name__ == "__main__":
    a = float(input("Enter coefficient a: "))
    b = float(input("Enter coefficient b: "))
    c = float(input("Enter coefficient c: "))

    roots = solve_quadratic(a, b, c)
    if not roots:
        print("No real roots")
    else:
        print(f"Roots: {roots}")
