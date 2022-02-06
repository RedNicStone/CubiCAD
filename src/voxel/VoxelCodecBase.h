//
// Created by nic on 04/10/2021.
//

#pragma once

#ifndef CUBICAD_VOXELCODECBASE_H_
#define CUBICAD_VOXELCODECBASE_H_

#include <memory>

#include "VoxelUtils.h"
#include "AllocationHandler.h"


struct VoxelRegion {    //! region of voxels, represents a three dimensional volume
    size_n x;             //!< x dimension
    size_n y;             //!< y dimension
    size_n z;             //!< z dimension
};

struct VoxelPosition {    //! region of voxels, represents a three dimensional volume
    size_n x;             //!< x dimension
    size_n y;             //!< y dimension
    size_n z;             //!< z dimension
};

class VoxelCodecBase {  //! base for all voxel codecs, this represents voxels stored in some way
  protected:
    VoxelRegion allocationSize{};     //!< region of voxels stored
    Allocator *allocationHandler{};     //!< pointer to AllocationHandler that handles
    //!< allocations

    void *allocationPtr{};     //!< pointer to the storage allocation
    size_v size{};     //!< size of the storage allocation

    void *readWritePtr{};     //!< pointer to the point in memory that is being read
    size_v voxelIndex{};     //!< index of the current voxel

  public:
    void *getDataPtr() { return allocationPtr; }  //!< returns the raw storage pointer
    void *getReadWritePtr() { return readWritePtr; }  //!< returns the read write pointer

    [[nodiscard]]
    size_v getCurrentVoxelIndex() const { return voxelIndex; }  //!< returns the current voxel index

    virtual void skipVoxel() {};     //!< proceeds to the next voxel
    virtual void skipVoxels(size_v count) {};     //!< skips n voxels
    virtual void jumpToVoxel(size_v index) {};     //!< jumps to voxel with specified index
    virtual void jumpToVoxel(VoxelPosition pos);     //!< jumps to voxel at position pos

    virtual void *getElementPtr() {};     //!< returns pointer to the current element
    virtual void copyElementToPtr(void *dst) {};     //!< copies the current element to the given destination

    virtual void copyElementFromPtr(void *src) {};     //!< copies data from pointer to current element

    virtual             ~VoxelCodecBase() = default;     //!< virtual destructor
};

void VoxelCodecBase::jumpToVoxel(VoxelPosition pos) {
    jumpToVoxel(libmorton::morton3D_encode(pos.x, pos.y, pos.z));
}

#endif //CUBICADVOXELTYPES__VOXELCODECBASE_H_
