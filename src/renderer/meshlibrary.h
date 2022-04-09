//
// Created by nic on 25/01/2022.
//

#pragma once

#ifndef CUBICAD_MESHLIBRARY_H
#define CUBICAD_MESHLIBRARY_H

#include "mesh.h"
#include "modelloader.h"


/// Library for handing mesh creation
class MeshLibrary {
  private:
    std::shared_ptr<ModelLoader> modelLoader;

    std::unordered_map<std::string, std::vector<std::shared_ptr<Mesh>>> meshes;

  public:
    /// Create a new mesh library
    /// \param textureLibrary Handle to the texture library to use
    /// \param materialLibrary Handle to the material library to use
    /// \return Valid MeshLibrary handle
    static std::shared_ptr<MeshLibrary> create(const std::shared_ptr<TextureLibrary> &textureLibrary,
                                               const std::shared_ptr<MaterialLibrary> &materialLibrary);

    /// Import a model from a given file
    /// \param filename Location of the file to load from
    /// \param masterMaterialTemplate Template for materials that will be created when loading the model
    /// \param normalizePos Bool flag that normalizes the model position to the origin
    /// \return Vector of newly created meshes from the given file
    std::vector<std::shared_ptr<Mesh>> createMesh(const std::string &filename,
                                                  const std::shared_ptr<MasterMaterialTemplate> &material,
                                                  bool normalizePos = false);

    std::unordered_map<std::string, std::vector<std::shared_ptr<Mesh>>> getMeshes() { return meshes; }
};

#endif //CUBICAD_MESHLIBRARY_H
