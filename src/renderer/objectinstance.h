//
// Created by nic on 09/01/2022.
//

#pragma once

#ifndef CUBICAD_OBJECTINSTANCE_H
#define CUBICAD_OBJECTINSTANCE_H

#include "mesh.h"


class Mesh;

struct InstanceData {
    alignas(4) glm::uint objectID;
    alignas(64) glm::mat4 model;
};

class MeshInstance {
  private:
    std::shared_ptr<Mesh> mesh;

    glm::vec3 pos;
    glm::vec3 rot;
    glm::vec3 scale;

    InstanceData instanceData;

  public:
    static std::shared_ptr<MeshInstance> create(std::shared_ptr<Mesh> masterMesh);

    std::shared_ptr<Mesh> getMesh() { return mesh; }
    InstanceData getInstanceData() { return instanceData; }
};

#endif //CUBICAD_OBJECTINSTANCE_H
