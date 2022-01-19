//
// Created by nic on 19/01/2022.
//

#ifndef CUBICAD_UIRENDERER_H
#define CUBICAD_UIRENDERER_H

#include "../vulkan/descriptorset.h"
#include <imgui.h>

class UIRenderer {
  private:
    std::shared_ptr<DescriptorPool> ImGUIPool;

    std::shared_ptr<RenderPass> renderPass;

  public:
    std::shared_ptr<UIRenderer> create(std::shared_ptr<Queue> graphicsQueue, std::shared_ptr<Queue> transferQueue,
                                       std::shared_ptr<Device> device);
};

#endif //CUBICAD_UIRENDERER_H
