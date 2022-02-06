#include <iostream>

#include "../voxel/VoxelFragmentTypes.h"
#include "../voxel/VoxelCodecLinear.h"


int main() {
    using fragment = VoxelFragment<uint16_t, uint32_t, uint64_t>;
    std::cout << fragment::getFragmentOffset<0>() << std::endl;
    std::cout << fragment::getFragmentOffset<1>() << std::endl;
    std::cout << fragment::getFragmentOffset<2>() << std::endl;
    std::cout << fragment::getFragmentSize<0>() << std::endl;
    std::cout << fragment::getFragmentSize<1>() << std::endl;
    std::cout << fragment::getFragmentSize<2>() << std::endl;
    std::cout << fragment::getFragmentTotalSize() << std::endl;
    std::cout << fragment::getFragmentCount() << std::endl;
    std::cout << sizeof(fragment::getFragmentType<0>()) << std::endl;

    AllocationHandler allocationHandler = AllocationHandler();
    auto linearCodec = VoxelCodecLinear<uint16_t, uint32_t, uint64_t>::Allocate(&allocationHandler, {8, 8, 8});
    std::cout << *static_cast<uint64_t *>(linearCodec->getElementPtr());

    return 0;
}
