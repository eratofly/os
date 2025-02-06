#define CATCH_CONFIG_MAIN

#include "catch2/catch_test_macros.hpp"
#include <cstddef> // для std::max_align_t
#include <thread>
#include "../src/MemoryManager.cpp"

TEST_CASE("MemoryManager") {
    constexpr size_t memorySize = 1024; // 1 KB
    alignas(std::max_align_t) char memory[memorySize];

    MemoryManager memoryManager(memory, memorySize);

    SECTION("Memory allocation and deallocation") {
        auto ptr = memoryManager.Allocate(sizeof(double));
        assert(ptr != nullptr);

        auto value = std::construct_at(static_cast<double*>(ptr), 3.1415927);
        assert(*value == 3.1415927);

        memoryManager.Free(ptr);
    }

    SECTION("Multiple memory allocations") {
        auto ptr1 = memoryManager.Allocate(100);
        auto ptr2 = memoryManager.Allocate(200);
        assert(ptr1 != nullptr && ptr2 != nullptr);

        memoryManager.Free(ptr1);
        memoryManager.Free(ptr2);
    }

    SECTION("An attempt to allocate too large a block") {
        void* ptr1 = memoryManager.Allocate(512);
        REQUIRE(ptr1 != nullptr);

        void* ptr2 = memoryManager.Allocate(600); // Больше, чем оставшаяся память
        REQUIRE(ptr2 == nullptr);
    }
}