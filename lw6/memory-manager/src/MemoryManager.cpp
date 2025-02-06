#include <cassert>
#include <cstddef>
#include <new>
#include <memory>
#include <mutex>
#include <utility>
#include <vector>

class MemoryManager
{
public:
    MemoryManager(void* start, size_t size) noexcept
            : m_start(static_cast<char*>(start)), m_size(size), m_freeMemory(size)
    {
        // проверка, что начальный адрес выровнен корректно.
        assert(reinterpret_cast<uintptr_t>(start) % alignof(std::max_align_t) == 0);
    }

    // удаляем конструктор копирования и оператор присваивания.
    MemoryManager(const MemoryManager&) = delete;
    MemoryManager& operator=(const MemoryManager&) = delete;

    //allocate выделяет блок памяти заданного размера и выравнивания.
    void* Allocate(size_t size, size_t align = alignof(std::max_align_t)) noexcept
    {
        std::lock_guard<std::mutex> lock(m_mutex); //потокобезопасность.

        //проверка, что выравнивание является степенью двойки.
        if ((align & (align - 1)) != 0 || size == 0)
            return nullptr;

        // находим первый подходящий свободный блок памяти.
        uintptr_t currentAddr = reinterpret_cast<uintptr_t>(m_start) + m_offset;
        uintptr_t alignedAddr = (currentAddr + (align - 1)) & ~(align - 1);

        size_t padding = alignedAddr - currentAddr;
        if (m_offset + padding + size > m_size)
            return nullptr; // это если памяти недостаточно

        m_offset += padding + size; // обновление смещения
        m_allocations.emplace_back(alignedAddr, size); // информация о выделении!

        return reinterpret_cast<void*>(alignedAddr);
    }
    //если освобождается, то вставлять туда же, а  не в конец

    // Метод Free освобождает ранее выделенный блок памяти.
    void Free(void* addr) noexcept
    {
        if (!addr)
            return; //если указатель нулевой, ничего не делаем

        std::lock_guard<std::mutex> lock(m_mutex);

        // Ищем указанный блок в списке выделений
        auto it = std::find_if(m_allocations.begin(), m_allocations.end(),
                               [addr](const Allocation& alloc) {
                                   return alloc.address == reinterpret_cast<uintptr_t>(addr);
                               });

        if (it != m_allocations.end())
        {
            m_freeMemory += it->size; // увеличение количества свободной памяти
            m_allocations.erase(it);  // Удаляем блок из списка выделений
        }
    }

private:
    struct Allocation
    {
        uintptr_t address;
        size_t size;
        Allocation(uintptr_t addr, size_t sz) : address(addr), size(sz) {}
    };

    char* m_start;                  // Начальный адрес блока памяти
    size_t m_size;                  // Размер блока памяти
    size_t m_offset = 0;            // Текущее смещение внутри блока памяти
    size_t m_freeMemory;            // Количество свободной памяти
    std::vector<Allocation> m_allocations; // Список выделений
    std::mutex m_mutex;             // Мьютекс для обеспечения потокобезопасности
};

//выделять память в том же месте, а не использовать список выделений
