//
// Created by nic on 09/01/2022.
//

#pragma once

#ifndef CUBICAD_OBJECTINSTANCE_H
#define CUBICAD_OBJECTINSTANCE_H

#include "mesh.h"




class ObjectInstance {
  private:
    std::shared_ptr<Mesh> mesh;
    glm::vec3 pos;
    glm::vec3 rot;
};

#endif //CUBICAD_OBJECTINSTANCE_H
