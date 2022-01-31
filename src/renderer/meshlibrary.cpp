//
// Created by nic on 25/01/2022.
//

#include "meshlibrary.h"


std::shared_ptr<MeshLibrary> MeshLibrary::create() {
    auto meshLibrary = std::make_shared<MeshLibrary>();
    meshLibrary->modelLoader = ModelLoader::create();

    return meshLibrary;
}

std::vector<std::shared_ptr<Mesh>> MeshLibrary::createMesh(const std::string &filename,
                                                           const std::shared_ptr<Material>& material) {
    if (meshes[filename].empty()) {
        auto mesh = modelLoader->import(filename, material);
        meshes[filename] = mesh;
    }
    return meshes[filename];
}
