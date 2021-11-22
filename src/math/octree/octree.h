//
// Created by nic on 30/04/2021.
//

#ifndef CUBICAD_OCTREE_H
#define CUBICAD_OCTREE_H

#include <cstdint>
#include <glm/glm.hpp>


namespace CCAD_OT {

  enum OtNodeType : unsigned char {  // possible types of a OtNode
      OT_NODE_TYPE_NODE = 0, OT_NODE_TYPE_ROOT = 1, OT_NODE_TYPE_LEAF = 2, OT_NODE_TYPE_INVALID = 3
  };

  enum OtFlagPos : unsigned char {  // possible positions of a OtNode
      OT_FLAG_POS_LEFT_FRONT_TOP = 0b00000001,
      OT_FLAG_POS_RIGHT_FRONT_TOP = 0b00000010,
      OT_FLAG_POS_LEFT_BACK_TOP = 0b00000100,
      OT_FLAG_POS_RIGHT_BACK_TOP = 0b00001000,
      OT_FLAG_POS_LEFT_FRONT_BOTTOM = 0b00010000,
      OT_FLAG_POS_RIGHT_FRONT_BOTTOM = 0b00100000,
      OT_FLAG_POS_LEFT_BACK_BOTTOM = 0b01000000,
      OT_FLAG_POS_RIGHT_BACK_BOTTOM = 0b10000000
  };

  enum OtPos : uint8_t {  // possible positions of a OtNode
      OT_POS_LEFT_FRONT_TOP = 0,
      OT_POS_RIGHT_FRONT_TOP = 1,
      OT_POS_LEFT_BACK_TOP = 2,
      OT_POS_RIGHT_BACK_TOP = 3,
      OT_POS_LEFT_FRONT_BOTTOM = 4,
      OT_POS_RIGHT_FRONT_BOTTOM = 5,
      OT_POS_LEFT_BACK_BOTTOM = 6,
      OT_POS_RIGHT_BACK_BOTTOM = 7
  };

  enum OtDrawMode : uint8_t {  // possible draw modes for a OtNode
      OT_DRAW_MODE_NO_DRAW = 0,  // not drawn
      OT_DRAW_MODE_DOT = 1,  // only a dot drawn
      OT_DRAW_MODE_WIREFRAME = 2,  // only lines drawn
      OT_DRAW_MODE_SOLID = 3,  // drawn as non textured solid
      OT_DRAW_MODE_TEXTURED = 4,  // drawn as textured solid
      OT_DRAW_MODE_SHADED = 5,  // drawn shaded
      OT_DRAW_MODE_DEBUG = 6   // debug mode [implementation varies]
  };

  enum OtFlagInfoType : unsigned char {  // possible positions of a OtNode
      OT_FLAG_INFO_TYPE_NONE = 0,  // no info type
      OT_FLAG_INFO_TYPE_RENDERED = 1   // rendered info type
  };

  struct OtMatInfo {};

  struct OtMatInfoSolid : OtMatInfo {
      glm::mediump_vec3 color;
  };

  struct OtMatInfoTextured : OtMatInfo {
      uint16_t tex_index;
      glm::mediump_ivec2 uv;
  };

  struct OtMatInfoShaded : OtMatInfoTextured {
      uint16_t mat_type;
  };

  struct OtMatInfoDebug : OtMatInfo {
      uint16_t channel;
  };

  union OtMatInfoUnion {
      OtMatInfo empty;
      OtMatInfoSolid solid;
      OtMatInfoTextured textured;
      OtMatInfoShaded shaded;
      OtMatInfoDebug debug;
  };

  struct OtSurfaceInfo {  // surface info
      OtMatInfoUnion materialInfo;

      OtDrawMode drawMode;  // draw mode
  };

  struct OtNodeInfo {  // empty OtNode info
      virtual OtFlagInfoType getType();
      static constexpr OtFlagInfoType type = OT_FLAG_INFO_TYPE_NONE;
  };

  struct OtNodeInfoRendered : public OtNodeInfo {  // rendered OtNode info
      OtNodeInfoRendered() = default;

      explicit OtNodeInfoRendered(const OtNodeInfo &info) {}

      OtSurfaceInfo *surfaceInfo[6]{};  // pointer 6 surface infos or nullptr

      OtFlagInfoType getType() override;
      static constexpr OtFlagInfoType type = OT_FLAG_INFO_TYPE_RENDERED;
  };

  struct OtNodePrimitive {  // basic OtNode structure
      OtNodeInfo nodeInfo; // info about this OtNode

      virtual OtNodePrimitive *getChild(OtPos pos);
      virtual OtNodePrimitive *getChild(uint8_t pos);

      virtual void setChild(OtPos pos, OtNodePrimitive *node) {};

      virtual void setChild(uint8_t pos, OtNodePrimitive *node) {};

      [[nodiscard]] const OtNodeInfo &getInfo() const;
      void setInfo(CCAD_OT::OtNodeInfo info);

      virtual OtNodeType getType();
      static constexpr OtNodeType type = OT_NODE_TYPE_INVALID;
  };

  struct OtNode : public OtNodePrimitive {  // basic OtNode structure
      OtNodePrimitive *children[8]{};  // pointer to children
      OtNodeInfo nodeInfo; // info about this OtNode

      OtNode() = default;

      explicit OtNode(const OtNodePrimitive &node) { this->setInfo(node.getInfo()); }

      OtNodePrimitive *getChild(OtPos pos) override;
      OtNodePrimitive *getChild(uint8_t pos) override;

      void setChild(OtPos pos, OtNodePrimitive *node) override;
      void setChild(uint8_t pos, OtNodePrimitive *node) override;

      OtNodeType getType() override;
      static constexpr OtNodeType type = OT_NODE_TYPE_NODE;
  };

  struct OtNodeRoot : public OtNode {  // OtNodeRoot OtNode structure

      OtNodeType getType() override;
      static constexpr OtNodeType type = OT_NODE_TYPE_ROOT;
  };

  struct OtNodeLeaf : public OtNodePrimitive {  // basic OtNode structure
      OtNodeInfo nodeInfo; // info about this OtNode

      OtNodeType getType() override;
      static constexpr OtNodeType type = OT_NODE_TYPE_LEAF;
  };

  struct OtConstructOctreeInfo {
      uint32_t construction_depth = 0;  // how deep the octree shall go
      bool use_custom_node_template = false;  // use custom OtNode template?
      OtNodePrimitive *custom_node_template = nullptr;  // custom OtNode template
  };

  class OtOctree {
    private:
      OtNodeRoot *root_node = new OtNodeRoot();

    public:
      void ot_construct_full_octree(const OtConstructOctreeInfo &info);
      void ot_erase_recursive();

      const OtNodePrimitive *ot_find_closest(double x, double y, double z);

      const OtNodePrimitive *ot_create_leaf(double x, double y, double z, uint32_t depth);

    private:
      void recursive_erase_(CCAD_OT::OtNodePrimitive *node);
      void recursive_full_construct_(uint32_t depth, const OtNodePrimitive *node_template, OtNodePrimitive *node);

      const OtNodePrimitive *recursive_find_closest_(double x,
                                                     double y,
                                                     double z,
                                                     uint32_t depth,
                                                     OtNodePrimitive *node);

      const OtNodePrimitive *recursive_create_leaf_(double x,
                                                    double y,
                                                    double z,
                                                    uint32_t depth_current,
                                                    uint32_t depth_goal,
                                                    OtNodePrimitive *node);
  };
}

#endif //CUBICAD_OCTREE_H
