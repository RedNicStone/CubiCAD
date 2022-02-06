//
// Created by nic on 23/12/2021.
//

#pragma once

#ifndef CUBICAD_MESH_H
#define CUBICAD_MESH_H

#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEFAULT_ALIGNED_GENTYPES

#include <memory>
#include <glm/glm.hpp>
#include <glm/gtx/hash.hpp>
#include <vector>
#include <string>

#include "material.h"


class Material;

struct PBRMaterialParameters;

struct BoundingBox {
    glm::vec3 pos1;
    glm::vec3 pos2;
};

struct Vertex {
    glm::vec3 pos;
    glm::u16vec2 uv;

    bool operator==(const Vertex &other) const {
        return pos == other.pos && uv == other.uv;
    }
};

namespace std {
  template<>
  struct hash<Vertex> {
      size_t operator()(Vertex const &vertex) const {
          return (std::hash<glm::vec3>{}(vertex.pos) ^ (std::hash<glm::vec2>{}(vertex.uv) << 1));
      }
  };
}

struct Meshlet {
    std::vector<uint32_t> indexData{};
    std::vector<Vertex> vertexData{};
    std::shared_ptr<Material> material;
};

class Mesh {
  private:
    std::vector<std::shared_ptr<Meshlet>> subMeshes{};

    BoundingBox boundingBox;
    glm::vec3 mean;

    uint32_t firstIndex{};
    uint32_t firstVertex{};
    uint32_t indexCount{};
    uint32_t vertexCount{};

    std::string name;

  public:
    static std::shared_ptr<Mesh> create(const std::vector<std::shared_ptr<Meshlet>> &meshlets,
                                        const BoundingBox &bbox,
                                        const std::string &pName = "");

    void setName(const std::string &pName) { name = pName; }

    [[nodiscard]] uint32_t getIndexCount() const { return indexCount; }

    [[nodiscard]] uint32_t getVertexCount() const { return vertexCount; }

    [[nodiscard]] glm::vec3 getMean() const { return mean; }

    [[nodiscard]] BoundingBox getBoundingBox() const { return boundingBox; }

    std::string getName() { return name; }

    std::vector<std::shared_ptr<Meshlet>> &getMeshlets() { return subMeshes; }

    [[nodiscard]] VkDrawIndexedIndirectCommand getDrawCommand() const;
    void setOffsets(uint32_t vFirstIndex, uint32_t vFirstVertex);
};

#endif //CUBICAD_MESH_H
