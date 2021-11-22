//
// Created by nic on 31/10/2021.
//

#pragma once

#ifndef CUBICAD_VOXELCODECSPARSEOCTREE_H
#define CUBICAD_VOXELCODECSPARSEOCTREE_H

#include "VoxelCodecOctreeBase.h"

template<typename ...Types>
class VoxelCodecSparseOctree : public VoxelCodecOctreeBase {
    //! this class a sparse voxel octree codec, stored according to:
    //! https://research.nvidia.com/sites/default/files/pubs/2010-02_Efficient-Sparse-Voxel/laine2010tr1_paper.pdf
    //!
    //! here is a quick recap of how one node is stored:
    //! memory position: [ 0123456789ABCDE |  F  |  01234567  |  89ABCDEF  ]
    //! memory usage:    [  child pointer  | far | valid mask | leaf  mask ]
    //!
    //! since the final size is not known, and it could change dynamically, more memory is reserved than necessary at
    //! all times

  private:
    bool    isLeaf  {};     //!< is the current node a leaf?

  public:
    bool    isLeaf()    override    { return isLeaf; }

}


#endif //CUBICAD_VOXELCODECSPARSEOCTREE_H
