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
    std::vector<Meshlet> subMeshes;

  public:
    static std::shared_ptr<Mesh> create(const std::vector<Meshlet>& meshlets);
};

#endif //CUBICAD_MESH_H
