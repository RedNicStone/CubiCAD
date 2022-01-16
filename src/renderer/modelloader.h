//
// Created by nic on 29/12/2021.
//

#pragma once

#ifndef CUBICAD_MODELLOADER_H
#define CUBICAD_MODELLOADER_H

#include <tiny_obj_loader.h>
#include <iostream>

#include "mesh.h"


class ModelLoader {
  private:
    tinyobj::ObjReader reader;
    tinyobj::ObjReaderConfig config{};

  public:
    static std::shared_ptr<ModelLoader> create();

    std::vector<std::shared_ptr<Mesh>> import(const std::string& filename, const std::shared_ptr<Material>&
    material);
};

#endif //CUBICAD_MODELLOADER_H
