//
// Created by nic on 02/10/2021.
//

#pragma once

#ifndef CUBICAD_VOXELCODECSHUFFLE_H_
#define CUBICAD_VOXELCODECSHUFFLE_H_

#include <memory>
#include <cstring>

#include "VoxelFragmentTypes.h"
#include "VoxelCodecBase.h"
#include "AllocationHandler.h"
#include "VoxelFragmentStream.h"
#include "VoxelUtils.h"


template<typename T, T... S, typename F>
constexpr void for_sequence(std::integer_sequence<T, S...>, F f) {
    (static_cast<void>(f(std::integral_constant<T, S>{})), ...);
}

template<typename ...Types>
class VoxelCodecShuffle : public VoxelCodecBase {
    //! this class represents a shuffled voxel codec, stored like this:
    //!      n       data
    //!      1       [ <attribute0>,  <attribute1>,  ...,  <attributeN>,
    //!      2         <attribute0>,  <attribute1>,  ...,  <attributeN>,
    //!      3         <attribute0>,  <attribute1>,  ...,  <attributeN> ]
    //!
    //! this implementation uses just as much memory as "VoxelCodecLinear",
    //! but since the attributes are close together it should be faster to
    //! work on

  private:
    using fragment = VoxelFragment<Types...>;

  public:
    static std::shared_ptr<VoxelCodecShuffle<Types...>> Allocate(AllocationHandler *handler, VoxelRegion region);

    ~VoxelCodecShuffle() override;

    void skipVoxel() override;
    void skipVoxels(size_v count) override;
    void jumpToVoxel(size_v index) override;

    void *getElementPtr() override;
    void copyElementToPtr(void *dst) override;
    void copyElementFromPtr(void *src) override;
};

template<typename ...Types>
std::shared_ptr<VoxelCodecShuffle<Types...>> VoxelCodecShuffle<Types...>::Allocate(AllocationHandler *handler,
                                                                                   VoxelRegion region) {
    static_assert(std::is_class_v<fragment>, "Types must be a valid parameter pack for VoxelFragment");

    std::shared_ptr<VoxelCodecShuffle<Types...>> codec = std::make_shared<VoxelCodecShuffle<Types...>>();

    codec->allocationSize = region;
    codec->size = region.x * region.y * region.z;
    codec->allocationPtr = handler->makeAllocation(codec->size * fragment::getFragmentTotalSize());
    codec->allocationHandler = handler;
    codec->readWritePtr = codec->allocationPtr;

    return codec;
}

template<class ...Types>
VoxelCodecShuffle<Types...>::~VoxelCodecShuffle() {
    allocationHandler->freeAllocation(allocationPtr);
}

template<typename... Types>
void VoxelCodecShuffle<Types...>::skipVoxel() {
    voxelIndex++;
    readWritePtr = static_cast<char *>(allocationPtr) + voxelIndex * fragment::getFragmentTotalSize();
}

template<typename... Types>
void VoxelCodecShuffle<Types...>::skipVoxels(size_v count) {
    voxelIndex += count;
    readWritePtr = static_cast<char *>(allocationPtr) + voxelIndex * fragment::getFragmentTotalSize();
}

template<typename... Types>
void VoxelCodecShuffle<Types...>::jumpToVoxel(size_v index) {
    voxelIndex = index;
    readWritePtr = static_cast<char *>(allocationPtr) + voxelIndex * fragment::getFragmentTotalSize();
}

template<class ...Types>
void *VoxelCodecShuffle<Types...>::getElementPtr() {
    return readWritePtr;
}

template<typename... Types>
void VoxelCodecShuffle<Types...>::copyElementToPtr(void *dst) {
    memcpy(static_cast<char *>(dst), static_cast<char *>(readWritePtr), (fragment::getFragmentTotalSize()));
}

template<typename... Types>
void VoxelCodecShuffle<Types...>::copyElementFromPtr(void *src) {
    memcpy(static_cast<char *>(readWritePtr), static_cast<char *>(dst), (fragment::getFragmentTotalSize()));
}

#endif //CUBICAD_VOXELCODECSHUFFLE_H_
