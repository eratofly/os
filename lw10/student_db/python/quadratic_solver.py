# import ctypes
# import os
#
# # Загрузка динамической библиотеки
# lib_path = os.path.join(os.path.dirname(__file__), "../build/lib/QuadraticSolver.dll")
# quadratic_solver = ctypes.CDLL(lib_path)
#
# # Настройка типов аргументов и возвращаемого значения
# quadratic_solver.solve_quadratic.argtypes = [ctypes.c_double, ctypes.c_double, ctypes.c_double]
# quadratic_solver.solve_quadratic.restype = ctypes.POINTER(ctypes.c_double)
#
# def solve_quadratic(a, b, c):
#     # Вызов функции из библиотеки
#     result = quadratic_solver.solve_quadratic(a, b, c)
#     size = 2  # Максимум два корня
#     return [result[i] for i in range(size) if result[i] != 0]
#
# if __name__ == "__main__":
#     a = float(input("Enter coefficient a: "))
#     b = float(input("Enter coefficient b: "))
#     c = float(input("Enter coefficient c: "))
#
#     try:
#         roots = solve_quadratic(a, b, c)
#         if not roots:
#             print("No real roots.")
#         else:
#             print("Roots:", roots)
#     except Exception as e:
#         print("Error:", e)
