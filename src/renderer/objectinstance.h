//
// Created by nic on 09/01/2022.
//

#pragma once

#ifndef CUBICAD_OBJECTINSTANCE_H
#define CUBICAD_OBJECTINSTANCE_H

#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEFAULT_ALIGNED_GENTYPES

#include <glm/gtx/transform.hpp>

#include "mesh.h"


class Mesh;

struct InstanceData {
    glm::uint objectID;
    glm::mat4 model;
};

class MeshInstance {
  private:
    std::shared_ptr<Mesh> mesh;

    glm::vec3 pos = {0, 0, 0};
    glm::vec3 rot = {0, 0, 0};
    glm::vec3 scale = {1, 1, 1};
    uint32_t objectID;

  public:
    static std::shared_ptr<MeshInstance> create(std::shared_ptr<Mesh> masterMesh);

    std::shared_ptr<Mesh> getMesh() { return mesh; }
    [[nodiscard]] InstanceData getInstanceData() const;

    void setPosition(glm::vec3 position);
    void setRotation(glm::vec3 rotation);
    void setScale(glm::vec3 scaling);

    void move(glm::vec3 position);

    void setID(uint32_t id) { objectID = id; }

    glm::vec3 getPosition() { return pos; }
    glm::vec3 getRotation() { return rot; }
    glm::vec3 getScale() { return scale; }
};

#endif //CUBICAD_OBJECTINSTANCE_H
