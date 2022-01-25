//
// Created by nic on 31/12/2021.
//

#pragma once

#ifndef CUBICAD_MASTERMATERIAL_H
#define CUBICAD_MASTERMATERIAL_H

#include "vulkan/graphicspipeline.h"
#include "material.h"
#include "mesh.h"
#include "objectinstance.h"
#include "materialparameters.h"

#include <utility>


class Material;

struct Vertex;

struct PBRMaterialParameters;

class MasterMaterial {
  private:
    std::shared_ptr<Device> device;

    MaterialPropertyLayoutBuilt propertyLayout;

    VkExtent2D extent{};
    std::vector<std::shared_ptr<GraphicsShader>> shaders;
    std::shared_ptr<RenderPass> renderPass;
    std::shared_ptr<GraphicsPipeline> pipeline;
    std::shared_ptr<PipelineLayout> pipelineLayout;

    std::shared_ptr<DescriptorSetLayout> materialSetLayout;
    std::shared_ptr<DescriptorSetLayout> masterMaterialSetLayout;
    std::shared_ptr<DescriptorSet> masterMaterialSet;

    uint32_t colorBlendStates;

    std::string name;
    uint32_t subMaterials;

    void generateMaterialSetLayout();

  public:
    static std::shared_ptr<MasterMaterial> create(const std::shared_ptr<Device>& pDevice, const
    std::vector<std::shared_ptr<GraphicsShader>>& vShaders, uint32_t vColorBlendStates, VkExtent2D vExtent, const MaterialPropertyLayout& layout, const
                                                  std::shared_ptr<RenderPass>&
    pRenderPass, const std::string& pName);

    void setName(const std::string& pName) { name = pName; }

    void updateDescriptorSetLayouts(const std::shared_ptr<DescriptorSetLayout>& sceneLayout, bool
    enableDepthStencil = false);

    size_t getParameterSize() { return propertyLayout.totalSize; }

    std::string getName() { return name; }
    std::string generateMaterialName();

    std::shared_ptr<GraphicsPipeline> getPipeline() { return pipeline; };
    std::shared_ptr<PipelineLayout> getPipelineLayout() { return pipelineLayout; };
    std::shared_ptr<DescriptorSet> getDescriptorSet() { return masterMaterialSet; };
    std::shared_ptr<RenderPass> getRenderPass() { return renderPass; };
};

#endif //CUBICAD_MASTERMATERIAL_H
