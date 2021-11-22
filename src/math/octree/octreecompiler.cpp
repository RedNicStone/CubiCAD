//
// Created by nic on 07/05/2021.
//

#include "octreecompiler.h"


namespace CCAD_OT {

  template<IndexType T_index, typename T_vertex> requires VertexMemberCheck<T_vertex>::value

  void OtGeometryCompiler<T_index, T_vertex>::recursive_iterate_nodes_(OtNodePrimitive &node,
                                                                       int pos_x,
                                                                       int pos_z,
                                                                       int pos_y,
                                                                       int level) {
      const OtNodeType type = node.getType();
      if (type & OT_NODE_TYPE_INVALID)
          return;

      OtNodeInfo node_info = node.getInfo();
      if (node_info.getType() & OT_FLAG_INFO_TYPE_RENDERED)
          for (const auto surface_info : static_cast<OtNodeInfoRendered>(node_info).surfaceInfo)
              if (surface_info != nullptr)
                  generate_data_(surface_info, pos_x, pos_y, pos_z, level);

      if (type & (OT_NODE_TYPE_ROOT | OT_NODE_TYPE_NODE))
          for (uint8_t i = 0; i < 8; i++) {
              OtNodePrimitive *child = node.getChild(i);
              if (child != nullptr)
                  recursive_iterate_nodes_(child, pos_x * 2 + i & 1, pos_x * 2 + i & 2, pos_x * 2 + i & 4, level + 1);
          }
  }

  template<IndexType T_index, typename T_vertex> requires VertexMemberCheck<T_vertex>::value

  void OtGeometryCompiler<T_index, T_vertex>::generate_data_(const OtSurfaceInfo &info,
                                                             int pos_x,
                                                             int pos_z,
                                                             int pos_y,
                                                             int level) {
      const double dx_ = 1.0 / (level * level);

      const double x_ = dx_ * pos_x;
      const double y_ = dx_ * pos_y;
      const double z_ = dx_ * pos_z;

      switch (info.drawMode) {
          case OT_DRAW_MODE_NO_DRAW:return;
          case OT_DRAW_MODE_DOT: {
              T_vertex vertex = T_vertex();
              vertex.pos = glm::vec3(x_ + dx_ / 2, y_ + dx_ / 2, z_ + dx_ / 2);
              vertex.color = info.materialInfo.solid.color;
              indexDataPoints.push_back(vertexData.size());
              vertexData.push_back(vertex);
              return;
          }
          case OT_DRAW_MODE_WIREFRAME: {
              size_t index_offset = vertexData.size();
              for (auto &index : Cube::line_indices)
                  indexDataLines.push_back(index + index_offset);
              for (auto &coord : Cube::vertices) {
                  T_vertex vertex = T_vertex();
                  vertex.pos = coord;
                  vertex.color = info.materialInfo.solid.color;
                  vertexData.push_back(vertex);
              }
              return;
          }
          case OT_DRAW_MODE_SOLID: {
              size_t index_offset = vertexData.size();
              for (auto &index : Cube::face_indices)
                  indexDataFaces.push_back(index + index_offset);
              for (auto &coord : Cube::vertices) {
                  T_vertex vertex = T_vertex();
                  vertex.pos = coord;
                  vertex.color = info.materialInfo.solid.color;
                  vertexData.push_back(vertex);
              }
              return;
          }
          case OT_DRAW_MODE_TEXTURED: {
              size_t index_offset = vertexData.size();
              for (auto &index : Cube::face_indices)
                  indexDataFaces.push_back(index + index_offset);
              for (auto &coord : Cube::vertices) {
                  T_vertex vertex = T_vertex();
                  vertex.pos = coord;
                  vertex.color = info.materialInfo.textured.uv;  // todo tex index
                  vertexData.push_back(vertex);
              }
              return;
          }
          case OT_DRAW_MODE_SHADED: {
              size_t index_offset = vertexData.size();
              for (auto &index : Cube::face_indices)
                  indexDataFaces.push_back(index + index_offset);
              for (auto &coord : Cube::vertices) {
                  T_vertex vertex = T_vertex();
                  vertex.pos = coord;
                  vertex.color = info.materialInfo.shaded.uv;
                  vertex.mat = info.materialInfo.shaded.mat_type;
                  vertexData.push_back(vertex);
              }
              return;
          }
          case OT_DRAW_MODE_DEBUG: {
              size_t index_offset = vertexData.size();
              for (auto &index : Cube::face_indices)
                  indexDataFaces.push_back(index + index_offset);
              for (auto &coord : Cube::vertices) {
                  T_vertex vertex = T_vertex();
                  vertex.pos = coord;
                  vertex.mat = 1;  // todo debug channel
                  vertexData.push_back(vertex);
              }
              return;
          }
      }
  }

}
