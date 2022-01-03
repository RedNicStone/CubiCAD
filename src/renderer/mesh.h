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

class Meshlet {
  private:
    std::vector<Vertex> vertexData;
    std::shared_ptr<Material> material;

  public:
    static std::shared_ptr<Meshlet> create(std::string filename);
};

class Mesh {
  private:
    std::vector<std::shared_ptr<Meshlet>> vertexData;

  public:
    static std::shared_ptr<Mesh> create(std::string filename);
};

#endif //CUBICAD_MESH_H
