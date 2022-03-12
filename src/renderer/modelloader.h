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


class ModelLoader {
  private:
    tinyobj::ObjReader reader;
    tinyobj::ObjReaderConfig config{};

    std::shared_ptr<TextureLibrary> textureLibrary;
    std::shared_ptr<MaterialLibrary> materialLibrary;

    static void loadMaterialProperties(char *property,
                                       const MaterialPropertyBuiltGeneric *materialProperty,
                                       tinyobj::material_t material);

    static std::vector<std::shared_ptr<Texture>> loadMaterialTextures(const std::shared_ptr<MaterialPropertyLayoutBuilt> &materialLayout,
                                                                      const tinyobj::material_t &material,
                                                                      const std::shared_ptr<TextureLibrary> &textureLibrary,
                                                                      const std::string& modelFilename);

    static std::string locateTexture(const std::string &textureFilename,
                                     const std::string &modelFilename);

  public:
    static std::shared_ptr<ModelLoader> create(const std::shared_ptr<TextureLibrary> &textureLibrary,
                                               const std::shared_ptr<MaterialLibrary> &materialLibrary);

    std::vector<std::shared_ptr<Mesh>> import(const std::string &filename,
                                              const std::shared_ptr<MasterMaterialTemplate>
                                              &masterMaterialTemplate,
                                              bool normalizePos = false);
};

#endif //CUBICAD_MODELLOADER_H
