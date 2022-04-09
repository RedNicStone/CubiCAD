//
// Created by nic on 29/12/2021.
//

#pragma once

#ifndef CUBICAD_MODELLOADER_H
#define CUBICAD_MODELLOADER_H

#include <tiny_obj_loader.h>
#include <iostream>
#include <filesystem>

#include "mesh.h"


/// Class that handles loading objects form file
class ModelLoader {
  private:
    tinyobj::ObjReader reader;
    tinyobj::ObjReaderConfig config{};

    std::shared_ptr<TextureLibrary> textureLibrary;
    std::shared_ptr<MaterialLibrary> materialLibrary;

    /// Load materials properties from a tinyOBJ material struct
    /// \param property Name of the property to load
    /// \param materialProperty Pointer to the property
    /// \param material TinyOBJ material
    static void loadMaterialProperties(char *property,
                                       const MaterialPropertyBuiltGeneric *materialProperty,
                                       tinyobj::material_t material);

    /// Load material texture from a tinyOBJ material struct
    /// \param materialLayout Layout of the material to load for
    /// \param material TinyOBJ material
    /// \param textureLibrary Texture library handle to create new textures
    /// \param modelFilename File name of the model (for locating the textures)
    /// \return Vector of texture handles
    static std::vector<std::shared_ptr<Texture>> loadMaterialTextures(const std::shared_ptr<MaterialPropertyLayoutBuilt> &materialLayout,
                                                                      const tinyobj::material_t &material,
                                                                      const std::shared_ptr<TextureLibrary> &textureLibrary,
                                                                      const std::string& modelFilename);

    /// Locates a texture given the model location and texture path
    /// \param textureFilename Texture path (relative or absolute)
    /// \param modelFilename Model path (absolute)
    /// \return Location of the texture file if it exists
    static std::string locateTexture(const std::string &textureFilename,
                                     const std::string &modelFilename);

  public:
    /// Create a new ModelLoader
    /// \param textureLibrary Texture library for creating textures
    /// \param materialLibrary Material library for allocating materials
    /// \return Valid ModelLoader handle
    static std::shared_ptr<ModelLoader> create(const std::shared_ptr<TextureLibrary> &textureLibrary,
                                               const std::shared_ptr<MaterialLibrary> &materialLibrary);

    /// Import a model from a given file
    /// \param filename Location of the file to load from
    /// \param masterMaterialTemplate Template for materials that will be created when loading the model
    /// \param normalizePos Bool flag that normalizes the model position to the origin
    /// \return Vector of newly created meshes from the given file
    std::vector<std::shared_ptr<Mesh>> import(const std::string &filename,
                                              const std::shared_ptr<MasterMaterialTemplate>
                                              &masterMaterialTemplate,
                                              bool normalizePos = false);
};

#endif //CUBICAD_MODELLOADER_H
