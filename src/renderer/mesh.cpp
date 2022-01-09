//
// Created by nic on 23/12/2021.
//

#include "mesh.h"


std::shared_ptr<Mesh> Mesh::create(const std::vector<Meshlet>& meshlets) {
    auto mesh = std::make_shared<Mesh>();
    mesh->subMeshes = meshlets;
}
