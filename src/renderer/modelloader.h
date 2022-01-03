//
// Created by nic on 29/12/2021.
//

#ifndef CUBICAD_MODELLOADER_H
#define CUBICAD_MODELLOADER_H

#define TINYOBJLOADER_IMPLEMENTATION
#define TINYOBJLOADER_USE_MAPBOX_EARCUT

#include <tiny_obj_loader.h>

#include "mesh.h"


class ModelLoader {
  private:
    tinyobj::ObjReader reader;
    tinyobj::ObjReaderConfig config{};

  public:
    static std::shared_ptr<ModelLoader> create();

    std::shared_ptr<Mesh> import(const std::string& filename);
};

#endif //CUBICAD_MODELLOADER_H
