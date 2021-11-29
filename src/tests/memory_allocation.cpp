//
// Created by nic on 07/11/2021.
//

#include <chrono>
#include <iostream>

#include "../voxel/AllocationHandler.h"

int main() {
    auto t1 = std::chrono::high_resolution_clock::now();

    StackAllocator allocator = StackAllocator(20, 2);

    auto t2 = std::chrono::high_resolution_clock::now();

    VirtualStackAllocation* allocation = allocator.makeAllocation(1<<26);

    auto t3 = std::chrono::high_resolution_clock::now();

    for (size_t i = 0; i < (1<<26); i++)
        allocation->getData(i);

    auto t4 = std::chrono::high_resolution_clock::now();

    for (size_t i = 0; i < (1<<26); i++)
        *(allocation->getData(i)) = static_cast<char>(i);

    auto t5 = std::chrono::high_resolution_clock::now();

    bool valid = true;
    for (size_t i = 0; i < (1<<26); i++)
        valid &= *(allocation->getData(i)) == static_cast<char>(i);

    auto t6 = std::chrono::high_resolution_clock::now();

    allocation->resize(1<<24);

    auto t7 = std::chrono::high_resolution_clock::now();

    allocation->resize(1<<26);

    auto t8 = std::chrono::high_resolution_clock::now();


    std::cout << "Creation of the allocator took "
              << std::chrono::duration_cast<std::chrono::nanoseconds>(t2 - t1).count()
              << " ns"
              << std::endl;

    std::cout << "The allocation took "
              << std::chrono::duration_cast<std::chrono::microseconds>(t3 - t2).count()
              << " us"
              << std::endl;

    std::cout << "Addressing all blocks took "
              << std::chrono::duration_cast<std::chrono::nanoseconds>(t4 - t3).count() / (1<<26)
              << " ns on average"
              << std::endl;

    std::cout << "Writing to all blocks took "
              << std::chrono::duration_cast<std::chrono::nanoseconds>(t5 - t4).count() / (1<<26)
              << " ns on average"
              << std::endl;

    std::cout << "Validating all write processes took "
              << std::chrono::duration_cast<std::chrono::nanoseconds>(t6 - t5).count() / (1<<26)
              << " ns on average. Was the written data correct?: "
              << valid
              << std::endl;

    std::cout << "Decrasing the allocation size to x/4 took "
              << std::chrono::duration_cast<std::chrono::microseconds>(t7 - t6).count()
              << " us."
              << std::endl;

    std::cout << "Increasing the allocation size to x4 took "
              << std::chrono::duration_cast<std::chrono::microseconds>(t8 - t7).count()
              << " us."
              << std::endl;
}
