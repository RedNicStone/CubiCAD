//
// Created by nic on 30/04/2021.
//

#include "octree.h"

#include <utility>


namespace CCAD_OT {

  OtFlagInfoType OtNodeInfo::getType() {
      return type;
  }

  OtFlagInfoType OtNodeInfoRendered::getType() {
      return type;
  }

  OtNodePrimitive *OtNodePrimitive::getChild(OtPos pos) {
      return nullptr;
  }

  OtNodePrimitive *OtNodePrimitive::getChild(uint8_t pos) {
      return nullptr;
  }

  OtNodePrimitive *OtNode::getChild(OtPos pos) {
      return children[pos];
  }

  OtNodePrimitive *OtNode::getChild(uint8_t pos) {
      return children[pos];
  }

  void OtNode::setChild(OtPos pos, OtNodePrimitive *node) {
      children[pos] = node;
  }

  void OtNode::setChild(uint8_t pos, OtNodePrimitive *node) {
      children[pos] = node;
  }

  const OtNodeInfo &OtNodePrimitive::getInfo() const {
      return nodeInfo;
  }

  void OtNodePrimitive::setInfo(OtNodeInfo info) {
      nodeInfo = std::move(info);
  }

  OtNodeType OtNodePrimitive::getType() {
      return type;
  }

  OtNodeType OtNode::getType() {
      return type;
  }

  OtNodeType OtNodeRoot::getType() {
      return type;
  }

  OtNodeType OtNodeLeaf::getType() {
      return type;
  }

  void OtOctree::ot_construct_full_octree(const OtConstructOctreeInfo &info) {
      OtNodePrimitive *node_template;
      if (info.use_custom_node_template)
          node_template = info.custom_node_template;
      else
          node_template = new OtNode;

      recursive_full_construct_(info.construction_depth, node_template, root_node);
  }

  void OtOctree::ot_erase_recursive() {
      recursive_erase_(root_node);
  }

  const OtNodePrimitive *OtOctree::ot_find_closest(const double x, const double y, const double z) {
      return recursive_find_closest_(x, y, z, 1, root_node);
  }

  const OtNodePrimitive *OtOctree::ot_create_leaf(double x, double y, double z, uint32_t depth) {
      return recursive_create_leaf_(x, y, z, 1, depth, root_node);
  }

#pragma clang diagnostic push
#pragma ide diagnostic ignored "misc-no-recursion"

  void OtOctree::recursive_erase_(OtNodePrimitive *node) {
      for (uint8_t pos = 0; pos <= 8; pos++)
          if (node->getChild(pos))
              recursive_erase_(node->getChild(pos));
      delete node;
  }

  void OtOctree::recursive_full_construct_(const uint32_t depth,
                                           const OtNodePrimitive *node_template,
                                           OtNodePrimitive *node) {
      for (uint8_t pos = 0; pos <= 8; pos++) {
          if (depth <= 1)
              node->setChild(pos, new OtNodeLeaf);
          else {
              node->setChild(pos, new OtNode);
              recursive_full_construct_(depth - 1, node_template, node->getChild(pos));
          }
      }
  }

  const OtNodePrimitive *OtOctree::recursive_find_closest_(const double x,
                                                           const double y,
                                                           const double z,
                                                           const uint32_t depth,
                                                           OtNodePrimitive *node) {
      const double size = (0.5 / depth);
      const bool x_ = x > size;
      const bool y_ = y > size;
      const bool z_ = z > size;
      const uint8_t index = x_ + y_ * 2 + z_ * 4;

      OtNodePrimitive *next_node = node->getChild(index);
      if (next_node == nullptr)
          return node;
      if (next_node->getType() == OT_NODE_TYPE_LEAF)
          return next_node;

      return recursive_find_closest_(x_ ? x - size : x, y_ ? y - size : y, z_ ? z - size : z, depth + 1, next_node);
  }

  const OtNodePrimitive *OtOctree::recursive_create_leaf_(double x,
                                                          double y,
                                                          double z,
                                                          uint32_t depth_current,
                                                          uint32_t depth_goal,
                                                          OtNodePrimitive *node) {
      const double size = (0.5 / depth_current);
      const bool x_ = x > size;
      const bool y_ = y > size;
      const bool z_ = z > size;
      const uint8_t index = x_ + y_ * 2 + z_ * 4;

      if (depth_goal == depth_current) {
          node->setChild(index, new OtNodeLeaf);
          return node->getChild(index);
      }

      OtNodePrimitive *next_node = node->getChild(index);
      if (node->getChild(index) == nullptr) {
          node->setChild(index, new OtNode);
          next_node = node->getChild(index);
      } else if (next_node->getType() == OT_NODE_TYPE_LEAF) {
          OtNodeInfo info = node->getInfo();
          node->setChild(index, new OtNode);
          next_node = node->getChild(index);
          next_node->setInfo(info);
      }

      return recursive_create_leaf_(x_ ? x - size : x,
                                    y_ ? y - size : y,
                                    z_ ? z - size : z,
                                    depth_current + 1,
                                    depth_goal,
                                    next_node);
  }

#pragma clang diagnostic pop

}
