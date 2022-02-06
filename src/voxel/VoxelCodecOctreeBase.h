//
// Created by nic on 18/10/2021.
//

#pragma once

#ifndef CUBICAD_VOXELCODECOCTREEBASE_H_
#define CUBICAD_VOXELCODECOCTREEBASE_H_

#include <stack>
#include <boost/bimap.hpp>

#include "VoxelUtils.h"
#include "VoxelCodecBase.h"
#include "VoxelFragmentTypes.h"
#include "AllocationHandler.h"
#include "../utils/utils.h"


enum OctreePosFlag : uint_fast8_t {                 //!< positions of a octree as bitmask
    OCTREE_POS_FLAG_TOP_FRONT_RIGHT = 0b00000001,
    OCTREE_POS_FLAG_TOP_FRONT_LEFT = 0b00000010,
    OCTREE_POS_FLAG_TOP_BACK_RIGHT = 0b00000100,
    OCTREE_POS_FLAG_TOP_BACK_LEFT = 0b00001000,
    OCTREE_POS_FLAG_BACK_FRONT_RIGHT = 0b00010000,
    OCTREE_POS_FLAG_BACK_FRONT_LEFT = 0b00100000,
    OCTREE_POS_FLAG_BACK_BACK_RIGHT = 0b01000000,
    OCTREE_POS_FLAG_BACK_BACK_LEFT = 0b10000000,
};

enum OctreePos : uint_fast8_t {                     //!< positions of a octree as morton code
    OCTREE_POS_TOP_FRONT_RIGHT = 0b000,
    OCTREE_POS_TOP_FRONT_LEFT = 0b001,
    OCTREE_POS_TOP_BACK_RIGHT = 0b010,
    OCTREE_POS_TOP_BACK_LEFT = 0b011,
    OCTREE_POS_BACK_FRONT_RIGHT = 0b100,
    OCTREE_POS_BACK_FRONT_LEFT = 0b101,
    OCTREE_POS_BACK_BACK_RIGHT = 0b110,
    OCTREE_POS_BACK_BACK_LEFT = 0b111,
};

[[maybe_unused]] const auto
    OCTREE_POS_RELATION_MAP =
    Utils::makeBimap<OctreePosFlag, OctreePos>({{OCTREE_POS_FLAG_TOP_FRONT_RIGHT, OCTREE_POS_TOP_FRONT_RIGHT},
                                                {OCTREE_POS_FLAG_TOP_FRONT_LEFT, OCTREE_POS_TOP_FRONT_LEFT},
                                                {OCTREE_POS_FLAG_TOP_BACK_RIGHT, OCTREE_POS_TOP_BACK_RIGHT},
                                                {OCTREE_POS_FLAG_TOP_BACK_LEFT, OCTREE_POS_TOP_BACK_LEFT},
                                                {OCTREE_POS_FLAG_BACK_FRONT_RIGHT, OCTREE_POS_BACK_FRONT_RIGHT},
                                                {OCTREE_POS_FLAG_BACK_FRONT_LEFT, OCTREE_POS_BACK_FRONT_LEFT},
                                                {OCTREE_POS_FLAG_BACK_BACK_RIGHT, OCTREE_POS_BACK_BACK_RIGHT},
                                                {OCTREE_POS_FLAG_BACK_BACK_LEFT, OCTREE_POS_BACK_BACK_LEFT}});

class VoxelCodecOctreeBase : public VoxelCodecBase {
  protected:
    struct NodeStackElement {                       //! struct that represents a previously visited node
        size_t nodePtr;     //!< pointer to the actual node
        char *memoryPtr;
        OctreePos nodePos;     //!< stores the position of the node relative to the
        // previous one
    };

    size_v actualSize{};

    std::stack<NodeStackElement> nodeStack{};     //!< stack containing all previously visited nodes

    VirtualAllocation allocation;

  public:
    size_n getDepth() { return nodeStack.size(); }   //!< get the current depth
    OctreePos getPos() { return nodeStack.top().nodePos; }   //!< get the current pos

    virtual bool isLeaf() {};     //!< is the current voxel a leaf (has no children)?
    virtual bool holdsData() {};     //!< does the current voxel hold data?
    virtual OctreePosFlag getChild() {};

    virtual OctreePosFlag getLeaf() {};

    virtual bool enterNode() {};

    virtual bool enterNode(OctreePos pos) {};     //!< enter node at relative position
    virtual bool nextNode() {};

    virtual bool nextNode(OctreePos pos) {};

    virtual bool leaveNode() {};     //!< leave current node

    ~VoxelCodecOctreeBase() override = default;     //!< virtual destructor
};

class VoxelCodecOctreeConstructable : public VoxelCodecOctreeBase {
  public:

};

#endif //CUBICAD_VOXELCODECOCTREEBASE_H_
