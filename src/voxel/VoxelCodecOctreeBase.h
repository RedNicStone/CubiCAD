//
// Created by nic on 18/10/2021.
//

#pragma once

#ifndef CUBICAD_VOXELCODECOCTREEBASE_H_
#define CUBICAD_VOXELCODECOCTREEBASE_H_

#include <stack>

#include "VoxelUtils.h"
#include "VoxelCodecBase.h"
#include "VoxelFragmentTypes.h"


enum OctreePosFlag : uint_fast8_t {                 //!< positions of a octree as bitmask
    OCTREE_POS_FLAG_TOP_FRONT_RIGHT     = 0b00000001,
    OCTREE_POS_FLAG_TOP_FRONT_LEFT      = 0b00000010,
    OCTREE_POS_FLAG_TOP_BACK_RIGHT      = 0b00000100,
    OCTREE_POS_FLAG_TOP_BACK_LEFT       = 0b00001000,
    OCTREE_POS_FLAG_BACK_FRONT_RIGHT    = 0b00010000,
    OCTREE_POS_FLAG_BACK_FRONT_LEFT     = 0b00100000,
    OCTREE_POS_FLAG_BACK_BACK_RIGHT     = 0b01000000,
    OCTREE_POS_FLAG_BACK_BACK_LEFT      = 0b10000000,
};

enum OctreePos : uint_fast8_t {                     //!< positions of a octree as morton code
    OCTREE_POS_TOP_FRONT_RIGHT          = 0b000,
    OCTREE_POS_TOP_FRONT_LEFT           = 0b001,
    OCTREE_POS_TOP_BACK_RIGHT           = 0b010,
    OCTREE_POS_TOP_BACK_LEFT            = 0b011,
    OCTREE_POS_BACK_FRONT_RIGHT         = 0b100,
    OCTREE_POS_BACK_FRONT_LEFT          = 0b101,
    OCTREE_POS_BACK_BACK_RIGHT          = 0b110,
    OCTREE_POS_BACK_BACK_LEFT           = 0b111,
};

template<typename ...Types>
class VoxelCodecOctreeBase : public VoxelCodecBase {
  private:
    const static size_v MEMORY_REALLOCATION_TRIGGER = 0x7FFF8;  //!< trigger memory reallocation if memory exceeds
                                                                //!< (reservedMemory - MEMORY_REALLOCATION_TRIGGER -
                                                                //!< nodeSize)
                                                                //!< Default: 0x7FFF8 (65kB)
                                                                //!< !! THIS VALUE MUST ALWAYS BE LARGER THAN ONE NODE !!

    const static float  MEMORY_REALLOCATION_FACTOR  = 1.5;      //!< determines what size the new allocation will have
                                                                //!< newSize = oldSize * MEMORY_REALLOCATION_FACTOR
                                                                //!< Default: 1.5

    const static size_v MEMORY_INITIAL_ALLOCATION   = 0x7FFFF8; //!< size of the initial allocation
                                                                //!< Default: 0x7FFFF8 (1MB)

  protected:
    struct NodeStackElement {                       //! struct that represents a previously visited node
        void*                       nodePtr           ;     //!< pointer to the actual node
        OctreePos                   nodePos           ;     //!< stores the position of the node relative to the
        // previous one
    };

    size_v                          actualSize      {};

    std::stack<NodeStackElement>    nodeStack       {};     //!< stack containing all previously visited nodes

    void            expandAllocation()                ;

  public:
    size_n          getDepth()                      { return nodeStack.size();          }   //!< get the current depth
    OctreePos       getPos()                        { return nodeStack.top().nodePos;   }   //!< get the current pos

    virtual bool    isLeaf()                        {};     //!< is the current voxel a leaf (has no children)?
    virtual bool    holdsData()                     {};     //!< does the current voxel hold data?

    virtual void    enterNode(OctreePos pos)        {};     //!< enter node at relative position
    virtual void    leaveNode()                     {};     //!< leave current node

    virtual void    makeNode(OctreePos pos)         {};     //!< make current leaf a node

    ~VoxelCodecOctreeBase()         override = default;     //!< virtual destructor
};

template<typename... Types>
void VoxelCodecOctreeBase<Types...>::expandAllocation() {
    if (actualSize > size - (VoxelFragment<Types...>::getTotalFragmentSize() - MEMORY_REALLOCATION_TRIGGER)) {
        allocationHandler.
    }
}

#endif //CUBICAD_VOXELCODECOCTREEBASE_H_
