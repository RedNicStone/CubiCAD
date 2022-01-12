//
// Created by nic on 31/12/2021.
//

#include "mastermaterial.h"

template<typename Parameters>
std::shared_ptr<Material> MasterMaterial::create(const std::shared_ptr<Device>& pDevice, const
std::vector<std::shared_ptr<GraphicsShader>>& shaders, VkExtent2D extent) {
    auto material = std::make_shared<Material>();
    material->device = pDevice;
    material->extent = extent;
    material->shaders = shaders;
    material->renderPass = RenderPass::create(pDevice);

    return material;
}