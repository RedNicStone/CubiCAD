//
// Created by nic on 02/10/2021.
//

#pragma once

#ifndef CUBICAD_VOXELCODECLINEAR_H_
#define CUBICAD_VOXELCODECLINEAR_H_

#include <memory>
#include <cstring>

#include "VoxelFragmentTypes.h"
#include "VoxelCodecBase.h"
#include "AllocationHandler.h"
#include "VoxelFragmentStream.h"
#include "VoxelUtils.h"


template <typename T, T... S, typename F>
constexpr void for_sequence(std::integer_sequence<T, S...>, F f) {
    (static_cast<void>(f(std::integral_constant<T, S>{})), ...);
}

template<typename ...Types>
class VoxelCodecLinear : public VoxelCodecBase {
    //! this class represents a linear voxel codec, stored like this:
    //!      n       1               2               3
    //!      data    [ <attribute0>,  <attribute0>,  <attribute0>,
    //!                <attribute1>,  <attribute1>,  <attribute1>,
    //!                ...,
    //!                <attributeN>,  <attributeN>,  <attributeN> ]
    //!
    //! this implementation is probably slower than "VoxelCodecShuffle",
    //! but the attributes are kept separate which makes them easy to
    //! stream

  private:
    using       fragment =                      VoxelFragment<Types...>;

  public:
    static std::shared_ptr<VoxelCodecLinear<Types...>>
                Allocate(AllocationHandler* handler, VoxelRegion region);

                ~VoxelCodecLinear()             override;

    void        skipVoxel()                     override;
    void        skipVoxels(size_v count)        override;
    void        jumpToVoxel(size_v index)       override;

    void *      getElementPtr()                 override;
    void        copyElementToPtr(void * dst)    override;
    void        copyElementFromPtr(void *src)   override;
};

template<typename ...Types>
std::shared_ptr<VoxelCodecLinear<Types...>> VoxelCodecLinear<Types...>::Allocate(AllocationHandler* handler,
                                                                                 VoxelRegion region) {
    static_assert(std::is_class_v<fragment>, "Types must be a valid parameter pack for VoxelFragment");
    
    std::shared_ptr<VoxelCodecLinear<Types...>> codec = std::make_shared<VoxelCodecLinear<Types...>>();
    
    codec->allocationSize       = region;
    codec->size                 = region.x * region.y * region.z;
    codec->allocationPtr        = handler->makeAllocation(codec->size * fragment::getFragmentTotalSize());
    codec->allocationHandler    = handler;
    codec->readWritePtr         = codec->allocationPtr;

    return codec;
}

template<class ...Types>
VoxelCodecLinear<Types...>::~VoxelCodecLinear() {
    allocationHandler->freeAllocation(allocationPtr);
}

template<typename... Types>
void VoxelCodecLinear<Types...>::skipVoxel() {
    voxelIndex++;
    readWritePtr = static_cast<char *>(allocationPtr) + voxelIndex;
}

template<typename... Types>
void VoxelCodecLinear<Types...>::skipVoxels(size_v count) {
    voxelIndex += count;
    readWritePtr = static_cast<char *>(allocationPtr) + voxelIndex;
}

template<typename... Types>
void VoxelCodecLinear<Types...>::jumpToVoxel(size_v index) {
    voxelIndex = index;
    readWritePtr = static_cast<char *>(allocationPtr) + voxelIndex;
}

template<class ...Types>
void * VoxelCodecLinear<Types...>::getElementPtr() {
    char * res = new char[fragment::getFragmentTotalSize()];

    for_sequence(std::make_index_sequence<fragment::getFragmentCount()>(), [&](const auto N) {
      std::size_t offset = (fragment::template getFragmentOffset<N>());
      memcpy(res + offset, static_cast<char *>(readWritePtr) + size * offset,
             (fragment::template getFragmentSize<N>()));
    });

    return res;
}

template<typename... Types>
void VoxelCodecLinear<Types...>::copyElementToPtr(void * dst) {
    for_sequence(std::make_index_sequence<fragment::getFragmentCount()>(), [&](const auto N) {
      std::size_t offset = (fragment::template getFragmentOffset<N>());
      memcpy(static_cast<char *>(dst) + offset, static_cast<char *>(readWritePtr) + size * offset,
             (fragment::template getFragmentSize<N>()));
    });
}

template<typename... Types>
void VoxelCodecLinear<Types...>::copyElementFromPtr(void *src) {
    for_sequence(std::make_index_sequence<fragment::getFragmentCount()>(), [&](const auto N) {
      std::size_t offset = (fragment::template getFragmentOffset<N>());
      memcpy(static_cast<char *>(readWritePtr) + size * offset, static_cast<char *>(src) + offset,
             (fragment::template getFragmentSize<N>()));
    });
}

#endif //CUBICADVOXELTYPES__VOXELCODECLINEAR_H_
