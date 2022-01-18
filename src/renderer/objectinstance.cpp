//
// Created by nic on 09/01/2022.
//

#include "objectinstance.h"

#include <utility>


std::shared_ptr<MeshInstance> MeshInstance::create(std::shared_ptr<Mesh> masterMesh) {
    auto meshInstance = std::make_shared<MeshInstance>();
    meshInstance->mesh = std::move(masterMesh);
    meshInstance->objectID = 5;

    return meshInstance;
}

InstanceData MeshInstance::getInstanceData() const {
    InstanceData data{};
    data.objectID = objectID;
    //data.model = glm::translate(glm::mat4(1), pos) * glm::scale(glm::mat4(1), scale) * glm::rotate(90.f, rot);
    data.model = glm::rotate(rot.x, glm::vec3(1, 0, 0))
               * glm::rotate(rot.y, glm::vec3(0, 1, 0))
               * glm::rotate(rot.z, glm::vec3(0, 0, 1))
               * glm::translate(pos)
               * glm::scale(scale);

    return data;
}

void MeshInstance::setPosition(glm::vec3 position) {
    pos = position;
}

void MeshInstance::setRotation(glm::vec3 rotation) {
    rot = rotation;
}

void MeshInstance::setScale(glm::vec3 scaling) {
    scale = scaling;
}

void MeshInstance::move(glm::vec3 position) {
    pos += position;
}
