//
// Created by nic on 09/01/2022.
//

#include "objectinstance.h"

#include <utility>


std::shared_ptr<MeshInstance> MeshInstance::create(std::shared_ptr<Mesh> masterMesh) {
    auto meshInstance = std::make_shared<MeshInstance>();
    meshInstance->mesh = std::move(masterMesh);
}
