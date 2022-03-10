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

class TextureLibrary;

class DescriptorPoolManager;

class MasterMaterial {
  private:
    std::shared_ptr<Device> device;

    std::shared_ptr<MaterialPropertyLayoutBuilt> propertyLayout;

    VkExtent2D extent{};
    std::vector<std::shared_ptr<GraphicsShader>> shaders;
    std::shared_ptr<RenderPass> renderPass;
    std::shared_ptr<GraphicsPipeline> pipeline;
    std::shared_ptr<PipelineLayout> pipelineLayout;

    std::shared_ptr<DescriptorPoolManager> descriptorManager;
    std::shared_ptr<DescriptorSetLayout> materialSetLayout;
    std::shared_ptr<DescriptorSetLayout> masterMaterialSetLayout;
    std::shared_ptr<DescriptorSet> masterMaterialSet;

    uint32_t colorBlendStates;

    std::string name;
    uint32_t subMaterials;

    void generateMaterialSetLayout();

  public:
    static std::shared_ptr<MasterMaterial> create(const std::shared_ptr<Device> &pDevice,
                                                  const std::vector<std::shared_ptr<GraphicsShader>> &vShaders,
                                                  uint32_t vColorBlendStates,
                                                  VkExtent2D vExtent,
                                                  const std::shared_ptr<MaterialPropertyLayoutBuilt> &layout,
                                                  const std::shared_ptr<RenderPass> &pRenderPass,
                                                  const std::shared_ptr<DescriptorPoolManager> &descriptorManager,
                                                  const std::string &pName);

    void setName(const std::string &pName) { name = pName; }
    void updateDescriptorSetLayouts(const std::shared_ptr<DescriptorSetLayout> &sceneLayout,
                                    bool enableDepthStencil = false);
    void updateImageSampler(const std::shared_ptr<TextureLibrary> &textureLibrary);
    [[nodiscard]] size_t getPropertySize() const { return propertyLayout->totalSize; }
    std::shared_ptr<MaterialPropertyLayoutBuilt> getPropertyLayout() { return propertyLayout; }
    std::string getName() { return name; }

    std::string generateMaterialName();

    std::shared_ptr<GraphicsPipeline> getPipeline() { return pipeline; };
    std::shared_ptr<PipelineLayout> getPipelineLayout() { return pipelineLayout; };
    std::shared_ptr<DescriptorSetLayout> getMasterDescriptorSetLayout() { return masterMaterialSetLayout; };
    std::shared_ptr<DescriptorSetLayout> getMaterialDescriptorSetLayout() { return materialSetLayout; };
    std::shared_ptr<DescriptorSet> getDescriptorSet() { return masterMaterialSet; };
    std::shared_ptr<RenderPass> getRenderPass() { return renderPass; };
    std::shared_ptr<DescriptorPoolManager> getDescriptorManager() { return descriptorManager; };
};

#endif //CUBICAD_MASTERMATERIAL_H
