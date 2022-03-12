//
// Created by nic on 25/01/2022.
//

#pragma once

#ifndef CUBICAD_MESHLIBRARY_H
#define CUBICAD_MESHLIBRARY_H

#include "mesh.h"
#include "modelloader.h"


class MeshLibrary {
  private:
    std::shared_ptr<ModelLoader> modelLoader;

    std::unordered_map<std::string, std::vector<std::shared_ptr<Mesh>>> meshes;

  public:
    static std::shared_ptr<MeshLibrary> create(const std::shared_ptr<TextureLibrary> &textureLibrary,
                                               const std::shared_ptr<MaterialLibrary> &materialLibrary);

    std::vector<std::shared_ptr<Mesh>> createMesh(const std::string &filename,
                                                  const std::shared_ptr<MasterMaterialTemplate> &material,
                                                  bool normalizePos = false);

    std::unordered_map<std::string, std::vector<std::shared_ptr<Mesh>>> getMeshes() { return meshes; }
};

#endif //CUBICAD_MESHLIBRARY_H
