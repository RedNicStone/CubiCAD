//
// Created by nic on 28/01/2022.
//

#include "rendermanager.h"


std::shared_ptr<RenderManager> RenderManager::create() {
    auto renderData = std::make_shared<RenderManager>();

    renderData->materialLibrary = MaterialLibrary::create(pDevice, );

    return renderData;
}
