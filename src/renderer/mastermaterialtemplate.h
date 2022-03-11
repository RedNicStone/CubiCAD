//
// Created by nic on 11/03/2022.
//

#ifndef CUBICAD_MASTERMATERIALTEMPLATE_H
#define CUBICAD_MASTERMATERIALTEMPLATE_H

#include "vulkan/graphicspipeline.h"
#include "materialparameters.h"

class MasterMaterialTemplate {
  private:
    std::shared_ptr<MaterialPropertyLayoutBuilt> propertyLayout;
    std::vector<std::shared_ptr<GraphicsShader>> shaders;

};

#endif //CUBICAD_MASTERMATERIALTEMPLATE_H
