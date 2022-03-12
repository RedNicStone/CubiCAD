//
// Created by nic on 11/03/2022.
//

#include "mastermaterialtemplate.h"


std::shared_ptr<MasterMaterialTemplate> MasterMaterialTemplate::create(const std::vector<std::shared_ptr<GraphicsShader>>& shaders,
                                                                       const
                                                                       std::shared_ptr<MaterialPropertyLayoutBuilt>&
                                                                       propertyLayout) {
    auto masterMaterialTemplate = std::make_shared<MasterMaterialTemplate>();
    masterMaterialTemplate->shaders = shaders;
    masterMaterialTemplate->propertyLayout = propertyLayout;

    return masterMaterialTemplate;
}
