//
// Created by nic on 07/05/2021.
//

#ifndef CUBICAD_OCTREE_COMPILER_H
#define CUBICAD_OCTREE_COMPILER_H

#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEFAULT_ALIGNED_GENTYPES

#include <glm/glm.hpp>

#include <cstdint>
#include <type_traits>
#include <vector>

#include "octree.h"


namespace CCAD_OT {
  struct Cube {
      constexpr static const glm::vec3
          vertices
      [
          8
      ] =
          {glm::vec3(0, 0, 0), glm::vec3(1, 0, 0), glm::vec3(0, 1, 0), glm::vec3(1, 1, 0), glm::vec3(0, 0, 1),
           glm::vec3(1, 0, 1), glm::vec3(0, 1, 1), glm::vec3(1, 1, 1)};

      constexpr static const uint32_t
          line_indices[24] = {0, 1, 0, 2, 0, 4, 1, 3, 1, 5, 2, 3, 2, 6, 3, 8, 4, 5, 4, 6, 5, 7, 6, 7};

      constexpr static const uint32_t
          face_indices
      [
          36
      ] =
          {0, 1, 2, 1, 2, 3, 4, 5, 6, 5, 6, 7, 0, 1, 4, 1, 4, 5, 2, 3, 6, 3, 6, 7, 0, 2, 4, 2, 4, 6, 1, 3, 5, 3, 5, 7};
  };

  template<typename T> concept IndexType = std::is_base_of_v<uint8_t, T>    // VK_INDEX_TYPE_UINT8_EXT
      || std::is_base_of_v<uint16_t, T>   // VK_INDEX_TYPE_UINT16
      || std::is_base_of_v<uint32_t, T>;  // VK_INDEX_TYPE_UINT32

  template<typename T, T>
  struct SFINAECheck;  // perform a SFINAE check

  struct VertexDataType {
      glm::mediump_vec3 pos;    // vertex position
      glm::mediump_vec3 color;  // color an UV are used in interchange
      uint16_t mat;    // material type
  };

  template<typename T> requires SFINAECheck<int T::*, &VertexDataType::pos>::value
  || SFINAECheck<int T::*, &VertexDataType::color>::value
  || SFINAECheck<int T::*, &VertexDataType::mat>::value
  struct VertexMemberCheck;  // perform member check (Ignoring types cuz im kinda lazy)


  template<IndexType T_index, typename T_vertex> requires VertexMemberCheck<T_vertex>::value

  class OtGeometryCompiler {
    private:
      T_index *pIndexBuffer;
      T_vertex *pVertexBuffer;

      std::vector<T_vertex> vertexData;

      std::vector<T_index> indexDataPoints;
      std::vector<T_index> indexDataLines;
      std::vector<T_index> indexDataFaces;

    public:
      void generateVertexData(const OtNodeRoot &node_root);

    private:
      void recursive_iterate_nodes_(OtNodePrimitive &node, int pos_x, int pos_z, int pos_y, int level);
      void generate_data_(const OtSurfaceInfo &info, int pos_x, int pos_z, int pos_y, int level);
  };
};

#endif //CUBICAD_OCTREE_COMPILER_H
