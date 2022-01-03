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


struct Vertex {
    glm::vec3 pos;
    glm::uvec2 uv;
};

class Mesh {
  private:
    std::vector<Vertex> vertexData;

  public:
    static std::shared_ptr<Mesh> create(std::string filename);
};

#endif //CUBICAD_MESH_H
