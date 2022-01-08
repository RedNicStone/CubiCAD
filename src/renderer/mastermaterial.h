//
// Created by nic on 31/12/2021.
//

#pragma once

#ifndef CUBICAD_MASTERMATERIAL_H
#define CUBICAD_MASTERMATERIAL_H

#include "vulkan/graphicspipeline.h"
#include "material.h"


template<typename Parameters>
class MasterMaterial {
  private:
    std::shared_ptr<PipelineLayout> pipelineLayout;
    std::shared_ptr<GraphicsPipeline> pipeline;

    Parameters defaultParameters;

  public:

};

typedef MasterMaterial<PBRMaterialParameters> PBRMasterMaterial;

#endif //CUBICAD_MASTERMATERIAL_H
