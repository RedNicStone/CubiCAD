//
// Created by nic on 09/01/2022.
//

#include "meshinstance.h"

#include <utility>


std::shared_ptr<MeshInstance> MeshInstance::create(const std::shared_ptr<Mesh> &masterMesh, const std::string &pName) {
    auto meshInstance = std::make_shared<MeshInstance>();
    meshInstance->mesh = masterMesh;
    if (pName.empty()) {
        meshInstance->name = masterMesh->getName();
    } else {
        meshInstance->name = pName;
    }

    meshInstance->pos = masterMesh->getMean();

    return meshInstance;
}

void MeshInstance::combineMatrices() {
    combined = matPos * matRot * matScale;
}

InstanceData MeshInstance::getInstanceData() const {
    InstanceData data{};
    data.objectID = objectID;
    data.model = combined;

    return data;
}

void MeshInstance::setPosition(glm::vec3 position) {
    pos = position;
    matPos = glm::translate(pos);
    combineMatrices();
}

void MeshInstance::setRotation(glm::vec3 rotation) {
    rot = rotation;
    matRot =
        glm::rotate(rot.x, glm::vec3(1, 0, 0))
            * glm::rotate(rot.y, glm::vec3(0, 1, 0))
            * glm::rotate(rot.z, glm::vec3(0, 0, 1));
    combineMatrices();
}

void MeshInstance::setScale(glm::vec3 scaling) {
    scale = scaling;
    matScale = glm::scale(scale);
    combineMatrices();
}

void MeshInstance::move(glm::vec3 position) {
    setPosition(pos + position);
}

void MeshInstance::setID(uint32_t id) {
    objectID = id;
    name += " [" + std::to_string(id) + "]";
}
