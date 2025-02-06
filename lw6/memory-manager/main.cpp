#include <cassert>
#include <vector>
#include "src/MemoryManager.cpp"

int main()
{
    alignas(std::max_align_t) char buffer[1000];
    MemoryManager memoryManager(buffer, sizeof(buffer));

    //Выделение и освобождение памяти.
    auto ptr = memoryManager.Allocate(sizeof(double));
    assert(ptr != nullptr);

    auto value = std::construct_at(static_cast<double*>(ptr), 3.1415927);
    assert(*value == 3.1415927);

    memoryManager.Free(ptr);

    //Несколько выделений.
    auto ptr1 = memoryManager.Allocate(100);
    auto ptr2 = memoryManager.Allocate(200);
    assert(ptr1 != nullptr && ptr2 != nullptr);

    memoryManager.Free(ptr1);
    memoryManager.Free(ptr2);

    //Попытка выделить слишком большой блок.
    auto largePtr = memoryManager.Allocate(2000);
    assert(largePtr == nullptr);

    return 0;
}
