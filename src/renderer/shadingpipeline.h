//
// Created by nic on 16/02/2022.
//

#pragma once

#ifndef CUBICAD_SHADINGPIPELINE_H
#define CUBICAD_SHADINGPIPELINE_H

#include "vulkan/graphicspipeline.h"

/// Shading pipeline used to apply shading effects
class ShadingPipeline {
  private:
    std::shared_ptr<GraphicsPipeline> pipeline;

    std::vector<std::shared_ptr<DescriptorSet>> descriptorSets;

  public:
    /// Create a new shading pipeline
    /// \param renderPass RenderPass where the pipeline should operate in
    /// \param descriptorSets Descriptor sets the shader has access to
    /// \param shader Shader to be used for the shading effects
    /// \param extend Extend of the render surface
    /// \param colorBlendStates Number of color blend states in the render pass
    /// \param subpass What subpass the effect is applied in
    /// \return Valid handle to ShadingPipeline object
    static std::shared_ptr<ShadingPipeline> create(const std::shared_ptr<RenderPass>& renderPass,
                                                   const std::vector<std::shared_ptr<DescriptorSet>>&
                                                   descriptorSets,
                                                   const std::shared_ptr<GraphicsShader>& shader,
                                                   const VkExtent2D& extend,
                                                   uint32_t colorBlendStates,
                                                   uint32_t subpass = 0);

    /// Bake all draw command to command buffer
    /// \param graphicsCommandBuffer Buffer to bake into
    void bakeGraphicsBuffer(const std::shared_ptr<CommandBuffer> &graphicsCommandBuffer);
};

#endif //CUBICAD_SHADINGPIPELINE_H
