//
// Created by nic on 31/10/2021.
//

#pragma once

#ifndef CUBICAD_VOXELCODECSPARSEOCTREE_H
#define CUBICAD_VOXELCODECSPARSEOCTREE_H

#include "VoxelCodecOctreeBase.h"
#include "../utils/utils.h"


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
    struct Node {
        uint16_t childPtr;
        uint8_t validMask;
        uint8_t leafMask;
    };

    bool leaf{};     //!< is the current node a leaf?

    void _enterNode();

  public:
    bool isLeaf() override { return isLeaf; }

    bool holdsData() override { return true; }

    OctreePosFlag getChild() override;
    OctreePosFlag getLeaf() override;

    bool enterNode() override;
    bool enterNode(OctreePos pos) override;
    bool nextNode() override;
    bool nextNode(OctreePos pos) override;
    bool leaveNode() override;

    bool makeNode() override;

};

template<typename... Types>
void VoxelCodecSparseOctree<Types...>::_enterNode() {
}

template<typename... Types>
OctreePosFlag VoxelCodecSparseOctree<Types...>::getChild() {
    return static_cast<OctreePosFlag>(*nodeStack.top().memoryPtr >> 8);
}

template<typename... Types>
OctreePosFlag VoxelCodecSparseOctree<Types...>::getLeaf() {
    return static_cast<OctreePosFlag>(*nodeStack.top().memoryPtr >> 0);
}

template<typename... Types>
bool VoxelCodecSparseOctree<Types...>::enterNode() {
    if (leaf)  // a leaf can't have children
        return false;

    Node *currentNode = *nodeStack.top();  // acquire current node (static)
    if (currentNode->validMask == 0x00)  // check any node exists
        return false;

    auto first_node = static_cast<OctreePos>(getMSB(currentNode->validMask));  // get first node position
    auto first_node_flag = static_cast<OctreePosFlag>((1 << first_node) >> 1);  // get first node pos as flag
    leaf = currentNode->leafMask & first_node_flag;  // check if the node will be a leaf

    uint_fast64_t pNextNode;  // pointer to next node
    if (currentNode->childPtr & 0x1) {  // childPtr is a far pointer
        pNextNode =
            (*allocation.getData(nodeStack.top().nodePtr + ((currentNode->childPtr >> 1) + 1) * 4) + 1)
                * 4;  // get value of far pointer
    } else {  // childPtr is a normal pointer
        pNextNode = (currentNode->childPtr >> 1) + 1;  // align with node pattern
    }
    pNextNode += nodeStack.top().nodePtr;  // add current pointer to nextNode pointer
    char *memPtr = allocation.getData(pNextNode);  // get physical pointer to nextNode
    nodeStack.push({pNextNode, memPtr, first_node});  // push node to stack
}

template<typename... Types>
bool VoxelCodecSparseOctree<Types...>::enterNode(OctreePos pos) {
    enterNode();

    nextNode(pos);
}

template<typename... Types>
bool VoxelCodecSparseOctree<Types...>::nextNode() {
    size_t pNextNode = nodeStack.top().nodePtr + 4;

    auto prev_node_bitmask = static_cast<uint_fast8_t>(0xFFu << prev_node_pos);
    nodeStack.pop();
    uint_fast8_t valid = (*nodeStack.top().memoryPtr >> 8);
    auto next_node = static_cast<OctreePos>(getMSB(valid & !prev_node_bitmask));
    leaf = (*nodeStack.top().memoryPtr) & (1 << next_node)

    char *memPtr = allocation.getData(nodeStack.top().nodePtr);
    nodeStack.push({pNextNode, memPtr, next_node});
}

template<typename... Types>
bool VoxelCodecSparseOctree<Types...>::nextNode(OctreePos pos) {
    while (nodeStack.top().nodePos != pos) {
        nextNode();
    }
}

template<typename... Types>
bool VoxelCodecSparseOctree<Types...>::leaveNode() {
    if (nodeStack.empty())
        return false;

    leaf = false;
    nodeStack.pop();
}

template<typename... Types>
bool VoxelCodecSparseOctree<Types...>::makeNode() {
    if (leaf)
        return false;
}

#endif //CUBICAD_VOXELCODECSPARSEOCTREE_H
