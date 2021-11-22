//
// Created by nic on 11/10/2021.
//

#pragma once

#ifndef CUBICAD_VOXELFRAGMENTSTREAM_H
#define CUBICAD_VOXELFRAGMENTSTREAM_H

#include <memory>

#include "VoxelUtils.h"


class VoxelFragmentStreamBase {
  protected:
    size_v          voxelIndex                          {};
    VoxelCodecBase  codec                               {};

  public:
    [[nodiscard]]
    size_v          getVoxelIndex() const               { return voxelIndex; }
    void            setVoxelIndex(size_v index)         { voxelIndex = index; }
    void            skipVoxel()                         { voxelIndex++; }
    void            skipVoxels(size_v count)            { voxelIndex += count; }

    virtual void *  getCurrentVoxelFragment()           {};
    virtual void *  getNextVoxelFragment()              {};

    virtual void *  refreshCurrentVoxelFragmentData()   {};

    virtual         ~VoxelFragmentStreamBase()          = default;;
};

template<class FragmentType>
class VoxelFragmentStreamBuffered : public VoxelFragmentStreamBase {
  private:
    void *  data{};

  public:
    void *  getCurrentVoxelFragment() override { return data; }
    void *  getNextVoxelFragment() override;

    void *  refreshCurrentVoxelFragmentData() override;
};

template<class FragmentType>
void *VoxelFragmentStreamBuffered<FragmentType>::getNextVoxelFragment() {
    voxelIndex++;
    refreshCurrentVoxelFragmentData();
    return data;
}

template<class FragmentType>
void *VoxelFragmentStreamBuffered<FragmentType>::refreshCurrentVoxelFragmentData() {

}

template<class FragmentTypeIn, class FragmentTypeOut>
class VoxelFragmentStreamTypeConverting : public VoxelFragmentStreamBuffered<FragmentTypeOut> {
  private:

};

#endif //CUBICADVOXELTYPES_VOXELFRAGMENTSTREAM_H
