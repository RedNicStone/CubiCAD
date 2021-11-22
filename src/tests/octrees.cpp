//
// Created by nic on 07/05/2021.
//

#include "../Math/Octree/octree.h"


int main() {
    CCAD_OT::OtOctree octree = CCAD_OT::OtOctree();

    //CCAD_OT::OtConstructOctreeInfo constructOctreeInfo{};
    //constructOctreeInfo.construction_depth = 4;
    //constructOctreeInfo.use_custom_node_template = false;
    //octree.ot_construct_full_octree(constructOctreeInfo);

    octree.ot_create_leaf(0.2, 0.3, 0.4, 5);
    octree.ot_create_leaf(0.5, 0.7, 0.2, 4);
    octree.ot_create_leaf(0.8, 0.9, 0.5, 3);

    octree.ot_erase_recursive();
}