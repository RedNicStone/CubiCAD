//
// Created by nic on 23/12/2021.
//

#include "mesh.h"


std::shared_ptr<Mesh> Mesh::create(const std::vector<std::shared_ptr<Meshlet>> &meshlets,
                                   const std::vector<Vertex> &vertexData,
                                   const BoundingBox &bbox,
                                   const std::string &pName,
                                   bool normalizePos) {
    auto mesh = std::make_shared<Mesh>();
    mesh->subMeshes = meshlets;

    mesh->boundingBox = bbox;
    if (normalizePos) {
        glm::vec3 mean = (bbox.min + bbox.max) / glm::vec3(2);
        mesh->mean = mean;
    }
    mesh->vertexData = vertexData;

    if (pName.empty())
        mesh->name = "UnnamedMesh";
    else
        mesh->name = pName;

    for (const auto &meshlet: meshlets) {
        mesh->indexCount += meshlet->indexData.size();
    }

    return mesh;
}

VkDrawIndexedIndirectCommand Mesh::getDrawCommand() const {
    VkDrawIndexedIndirectCommand command;
    command.indexCount = indexCount;
    command.firstIndex = firstIndex;
    command.vertexOffset = static_cast<int32_t>(firstVertex);

    return command;
}

void Mesh::setOffsets(uint32_t vFirstIndex, uint32_t vFirstVertex) {
    firstIndex = vFirstIndex;
    firstVertex = vFirstVertex;
}
