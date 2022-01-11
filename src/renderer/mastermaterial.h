//
// Created by nic on 31/12/2021.
//

#pragma once

#ifndef CUBICAD_MASTERMATERIAL_H
#define CUBICAD_MASTERMATERIAL_H

#include "vulkan/graphicspipeline.h"
#include "material.h"

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

  public:
    static std::shared_ptr<Material<Parameters>> create(const std::shared_ptr<Device>& pDevice, const
    std::vector<std::shared_ptr<GraphicsShader>>& shaders, VkExtent2D extent);

    void updateDescriptorPipelineLayouts(std::shared_ptr<PipelineLayout> pipelineLayout);
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
void MasterMaterial<Parameters>::updateDescriptorPipelineLayouts(std::shared_ptr<PipelineLayout> pipelineLayout) {
    pipeline = GraphicsPipeline::create(device, std::move(pipelineLayout), shaders, renderPass, {0, 0});
}

typedef MasterMaterial<PBRMaterialParameters> PBRMasterMaterial;

#endif //CUBICAD_MASTERMATERIAL_H
