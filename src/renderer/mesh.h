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
#include <vector>
#include <string>

#include "material.h"


class Material;

struct PBRMaterialParameters;

struct Vertex {
    glm::vec3 pos;
    glm::vec2 uv;
};

struct Meshlet {
    std::vector<uint32_t> indexData;
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

    std::string name;

  public:
    static std::shared_ptr<Mesh> create(const std::vector<std::shared_ptr<Meshlet>>& meshlets, const std::string& pName = "");

    void setName(const std::string& pName) { name = pName; }

    [[nodiscard]] uint32_t getIndexCount() const { return indexCount; }
    [[nodiscard]] uint32_t getVertexCount() const { return vertexCount; }

    std::string getName() { return name; }

    std::vector<std::shared_ptr<Meshlet>>& getMeshlets() { return subMeshes; }

    [[nodiscard]] VkDrawIndexedIndirectCommand getDrawCommand() const;
    void setOffsets(uint32_t vFirstIndex, uint32_t vFirstVertex);
};

#endif //CUBICAD_MESH_H
