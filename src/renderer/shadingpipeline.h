//
// Created by nic on 16/02/2022.
//

#pragma once

#ifndef CUBICAD_SHADINGPIPELINE_H
#define CUBICAD_SHADINGPIPELINE_H

#include "vulkan/graphicspipeline.h"

class ShadingPipeline {
  private:
    std::shared_ptr<GraphicsPipeline> pipeline;

    std::vector<std::shared_ptr<DescriptorSet>> descriptorSets;

  public:
    static std::shared_ptr<ShadingPipeline> create(const std::shared_ptr<RenderPass>& renderPass,
                                                   const std::vector<std::shared_ptr<DescriptorSet>>&
                                                   descriptorSets,
                                                   const std::shared_ptr<GraphicsShader>& shader,
                                                   const VkExtent2D& extend,
                                                   uint32_t colorBlendStates,
                                                   uint32_t subpass = 0);

    void bakeGraphicsBuffer(const std::shared_ptr<CommandBuffer> &graphicsCommandBuffer);
};

#endif //CUBICAD_SHADINGPIPELINE_H
