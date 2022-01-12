//
// Created by nic on 23/12/2021.
//

#pragma once

#ifndef CUBICAD_MESH_H
#define CUBICAD_MESH_H

#include <memory>
#include <glm/glm.hpp>
#include <vector>
#include <string>

#include "material.h"


class Material;

struct PBRMaterialParameters;

struct Vertex {
    glm::vec3 pos;
    glm::uvec2 uv;
};

struct Meshlet {
    std::vector<int> indexData;
    std::vector<Vertex> vertexData;
    std::shared_ptr<Material> material;
};

class Mesh {
  private:
    std::vector<std::shared_ptr<Meshlet>> subMeshes;

    uint32_t firstIndex;
    uint32_t firstVertex;
    uint32_t indexCount;
    uint32_t vertexCount;

  public:
    static std::shared_ptr<Mesh> create(const std::vector<std::shared_ptr<Meshlet>>& meshlets);

    [[nodiscard]] uint32_t getIndexCount() const { return indexCount; }
    [[nodiscard]] uint32_t getVertexCount() const { return vertexCount; }

    std::vector<std::shared_ptr<Meshlet>>& getMeshlets() { return subMeshes; }

    [[nodiscard]] VkDrawIndexedIndirectCommand getDrawCommand() const;
    void setOffsets(uint32_t vFirstIndex, uint32_t vFirstVertex);
};

#endif //CUBICAD_MESH_H
