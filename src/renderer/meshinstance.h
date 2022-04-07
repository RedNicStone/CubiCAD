//
// Created by nic on 09/01/2022.
//

#pragma once

#ifndef CUBICAD_MESHINSTANCE_H
#define CUBICAD_MESHINSTANCE_H

#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEFAULT_ALIGNED_GENTYPES

#include <glm/gtx/transform.hpp>
#include <utility>

#include "mesh.h"


class Mesh;

struct InstanceData {
    glm::mat4 model;
    glm::vec3 bbox1;
    glm::uint objectID;
    glm::vec3 bbox2;
    glm::uint batchID;
};

class MeshInstance {
  private:
    std::shared_ptr<Mesh> mesh;

    glm::vec3 pos = {0, 0, 0};
    glm::vec3 rot = {0, 0, 0};
    glm::vec3 scale = {1, 1, 1};

    glm::mat4 matRot = glm::mat4(1.0f);
    glm::mat4 matPos = glm::mat4(1.0f);
    glm::mat4 matScale = glm::mat4(1.0f);
    glm::mat4 combined = glm::mat4(1.0f);

    uint32_t objectID = 0;

    std::string name;

    void combineMatrices();

  public:
    static std::shared_ptr<MeshInstance> create(const std::shared_ptr<Mesh> &masterMesh, const std::string &pName = "");

    std::shared_ptr<Mesh> getMesh() { return mesh; }

    [[nodiscard]] InstanceData getInstanceData() const;

    void setPosition(glm::vec3 position);
    void setRotation(glm::vec3 rotation);
    void setScale(glm::vec3 scaling);

    void move(glm::vec3 position);

    void setID(uint32_t id);

    void setName(const std::string &pName) { name = pName; }

    glm::vec3 getPosition() { return pos; }

    glm::vec3 getRotation() { return rot; }

    glm::vec3 getScale() { return scale; }

    std::string getName() { return name; }

    [[nodiscard]] uint32_t getID() const { return objectID; }
};

#endif //CUBICAD_MESHINSTANCE_H
