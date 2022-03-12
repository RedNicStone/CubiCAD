//
// Created by nic on 11/03/2022.
//

#pragma once

#ifndef CUBICAD_MASTERMATERIALTEMPLATE_H
#define CUBICAD_MASTERMATERIALTEMPLATE_H

#include "vulkan/graphicspipeline.h"
#include "materialparameters.h"

class MasterMaterialTemplate {
  private:
    std::vector<std::shared_ptr<GraphicsShader>> shaders;
    std::shared_ptr<MaterialPropertyLayoutBuilt> propertyLayout;

  public:
    static std::shared_ptr<MasterMaterialTemplate> create(const std::vector<std::shared_ptr<GraphicsShader>>& shaders,
                                                   const std::shared_ptr<MaterialPropertyLayoutBuilt>& propertyLayout);

    std::vector<std::shared_ptr<GraphicsShader>> getShaders() { return shaders; }
    std::shared_ptr<MaterialPropertyLayoutBuilt> getPropertyLayout() { return propertyLayout; }
    [[nodiscard]] size_t getPropertySize() const { return propertyLayout->totalSize; }
};

#endif //CUBICAD_MASTERMATERIALTEMPLATE_H
