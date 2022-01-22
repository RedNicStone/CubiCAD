//
// Created by nic on 09/01/2022.
//

#include "objectinstance.h"

#include <utility>


std::shared_ptr<MeshInstance> MeshInstance::create(const std::shared_ptr<Mesh>& masterMesh, const std::string& pName) {
    auto meshInstance = std::make_shared<MeshInstance>();
    meshInstance->mesh = masterMesh;
    meshInstance->objectID = 5;
    if (pName.empty()) {
        meshInstance->name = masterMesh->getName();
    } else {
        meshInstance->name = pName;
    }

    return meshInstance;
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
    combined = matRot
             * matPos
             * matScale;
}

void MeshInstance::setRotation(glm::vec3 rotation) {
    rot = rotation;
    matRot = glm::rotate(rot.x, glm::vec3(1, 0, 0))
           * glm::rotate(rot.y, glm::vec3(0, 1, 0))
           * glm::rotate(rot.z, glm::vec3(0, 0, 1));
    combined = matRot
             * matPos
             * matScale;
}

void MeshInstance::setScale(glm::vec3 scaling) {
    scale = scaling;
    matScale = glm::scale(scale);
    combined = matRot
             * matPos
             * matScale;
}

void MeshInstance::move(glm::vec3 position) {
    pos += position;
}

void MeshInstance::setID(uint32_t id) {
    objectID = id;
    name += " [" + std::to_string(id) + "]";
}
