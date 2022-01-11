//
// Created by nic on 31/12/2021.
//

#pragma once

#ifndef CUBICAD_MASTERMATERIAL_H
#define CUBICAD_MASTERMATERIAL_H

#include "vulkan/graphicspipeline.h"
#include "material.h"
#include "mesh.h"

#include <utility>


template<typename Parameters>
class MasterMaterial {
  private:
    std::shared_ptr<Device> device;

    Parameters defaultParameters;

    VkExtent2D extent{};
    std::vector<std::shared_ptr<GraphicsShader>> shaders;
    std::shared_ptr<RenderPass> renderPass;
    std::shared_ptr<GraphicsPipeline> pipeline;
    std::shared_ptr<PipelineLayout> pipelineLayout;

    std::shared_ptr<DescriptorSetLayout> materialSetLayout;
    std::shared_ptr<DescriptorSetLayout> masterMaterialSetLayout;
    std::shared_ptr<DescriptorSet> masterMaterialSet;

  public:
    static std::shared_ptr<Material<Parameters>> create(const std::shared_ptr<Device>& pDevice, const
    std::vector<std::shared_ptr<GraphicsShader>>& shaders, VkExtent2D extent);

    void updateDescriptorSetLayouts(const std::shared_ptr<DescriptorSetLayout>& sceneLayout);

    std::shared_ptr<GraphicsPipeline> getPipeline() { return pipeline; };
    std::shared_ptr<PipelineLayout> getPipelineLayout() { return pipelineLayout; };
    std::shared_ptr<DescriptorSet> getDescriptorSet() { return masterMaterialSet; };
};

template<typename Parameters>
std::shared_ptr<Material<Parameters>> MasterMaterial<Parameters>::create(const std::shared_ptr<Device>& pDevice, const
std::vector<std::shared_ptr<GraphicsShader>>& shaders, VkExtent2D extent) {
    auto material = std::make_shared<Material<Parameters>>();
    material->device = pDevice;
    material->extent = extent;
    material->shaders = shaders;
    material->renderPass = RenderPass::create(pDevice);

    return material;
}

template<typename Parameters>
void MasterMaterial<Parameters>::updateDescriptorSetLayouts(const std::shared_ptr<DescriptorSetLayout>& sceneLayout) {
    std::vector<std::shared_ptr<DescriptorSetLayout>> descriptorLayouts{ sceneLayout, masterMaterialSetLayout,
                                                                         materialSetLayout };

    pipelineLayout = PipelineLayout::create(device, descriptorLayouts);

    std::vector<VkVertexInputBindingDescription> bindingDescription {
        { 0, sizeof(Vertex), VK_VERTEX_INPUT_RATE_VERTEX },
        { 1, sizeof(Instance), VK_VERTEX_INPUT_RATE_INSTANCE }
    };
    std::vector<VkVertexInputAttributeDescription> attributeDescription {
        {  }
    };
    pipeline = GraphicsPipeline::create(device, pipelineLayout, shaders, renderPass, extent,
                                        VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST, VK_CULL_MODE_BACK_BIT, VK_FRONT_FACE_CLOCKWISE);
}

typedef MasterMaterial<PBRMaterialParameters> PBRMasterMaterial;

#endif //CUBICAD_MASTERMATERIAL_H
