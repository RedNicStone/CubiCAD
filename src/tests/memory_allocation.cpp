//
// Created by nic on 07/11/2021.
//

#include <chrono>
#include <iostream>

#include "../voxel/AllocationHandler.h"

int main() {
    auto t1 = std::chrono::high_resolution_clock::now();

    StackAllocator allocator = StackAllocator(20);

    auto t2 = std::chrono::high_resolution_clock::now();

    VirtualStackAllocation* allocation = allocator.makeAllocation(1<<30);

    auto t3 = std::chrono::high_resolution_clock::now();

    for (size_t i = 0; i < (1<<30); i += 1)
        allocation->getData(i);

    auto t4 = std::chrono::high_resolution_clock::now();

    for (size_t i = 0; i < (1<<30); i += 1)
        *(allocation->getData(i)) = static_cast<char>(i);

    auto t5 = std::chrono::high_resolution_clock::now();

    bool valid = true;
    for (size_t i = 0; i < (1<<30); i += 1)
        valid &= *(allocation->getData(i)) == static_cast<char>(i);

    auto t6 = std::chrono::high_resolution_clock::now();


    std::cout << "Creation of the allocator took "
              << std::chrono::duration_cast<std::chrono::nanoseconds>(t2 - t1).count()
              << " ns"
              << std::endl;

    std::cout << "The allocation took "
              << std::chrono::duration_cast<std::chrono::microseconds>(t3 - t2).count()
              << " us"
              << std::endl;

    std::cout << "Addressing all blocks took "
              << std::chrono::duration_cast<std::chrono::nanoseconds>(t4 - t3).count() / (1<<30)
              << " ns on average"
              << std::endl;

    std::cout << "Writing to all blocks took "
              << std::chrono::duration_cast<std::chrono::nanoseconds>(t5 - t4).count() / (1<<30)
              << " ns on average"
              << std::endl;

    std::cout << "Validating all write processes took "
              << std::chrono::duration_cast<std::chrono::nanoseconds>(t6 - t5).count() / (1<<30)
              << " ns on average. Was the written data correct?: "
              << valid
              << std::endl;
}
