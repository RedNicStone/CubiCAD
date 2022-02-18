//
// Created by nic on 16/02/2022.
//

#include "shadingpipeline.h"


std::shared_ptr<ShadingPipeline> ShadingPipeline::create(const std::shared_ptr<RenderPass>& renderPass,
                                                         const std::vector<std::shared_ptr<DescriptorSet>>&
                                                         descriptorSets,
                                                         const std::shared_ptr<GraphicsShader>& shader,
                                                         const VkExtent2D& extend,
                                                         uint32_t colorBlendStates) {
    auto shadingPipeline = std::make_shared<ShadingPipeline>();

    auto vertexShader = VertexShader::create(renderPass->getDevice(), "main", "resources/compiled/quad.vert.spv");

    std::vector<std::shared_ptr<GraphicsShader>> shaders = {vertexShader, shader};

    std::vector<std::shared_ptr<DescriptorSetLayout>> descriptorSetLayouts(descriptorSets.size());
    for (size_t i = 0; i < descriptorSets.size(); ++i) {
        descriptorSetLayouts[i] = descriptorSets[i]->getLayout();
    }

    auto pipelineLayout = PipelineLayout::create(renderPass->getDevice(), descriptorSetLayouts);

    shadingPipeline->pipeline = GraphicsPipeline::create(renderPass->getDevice(), pipelineLayout, shaders, renderPass,
                                                         colorBlendStates, extend);

    shadingPipeline->descriptorSets = descriptorSets;

    return shadingPipeline;
}

void ShadingPipeline::bakeGraphicsBuffer(const std::shared_ptr<CommandBuffer> &graphicsCommandBuffer) {
    graphicsCommandBuffer->bindPipeline(pipeline);

    graphicsCommandBuffer->bindDescriptorSets(descriptorSets, pipeline);

    graphicsCommandBuffer->draw(6);
}
