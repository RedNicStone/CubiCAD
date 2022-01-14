//
// Created by nic on 31/12/2021.
//

#include "mastermaterial.h"


std::shared_ptr<MasterMaterial> MasterMaterial::create(const std::shared_ptr<Device>& pDevice, const
std::vector<std::shared_ptr<GraphicsShader>>& vShaders, VkExtent2D vExtent, const std::shared_ptr<RenderPass>&
pRenderPass) {
    auto material = std::make_shared<MasterMaterial>();
    material->device = pDevice;
    material->extent = vExtent;
    material->shaders = vShaders;
    material->renderPass = pRenderPass;

    return material;
}

void MasterMaterial::updateDescriptorSetLayouts(const std::shared_ptr<DescriptorSetLayout>& sceneLayout) {
    std::vector<std::shared_ptr<DescriptorSetLayout>> descriptorLayouts{ sceneLayout/*, masterMaterialSetLayout,
                                                                         materialSetLayout*/ }; //todo

    pipelineLayout = PipelineLayout::create(device, descriptorLayouts);

    std::vector<VkVertexInputBindingDescription> bindingDescription {
        { 0, sizeof(Vertex), VK_VERTEX_INPUT_RATE_VERTEX },
        { 1, sizeof(InstanceData), VK_VERTEX_INPUT_RATE_INSTANCE }
    };
    std::vector<VkVertexInputAttributeDescription> attributeDescription {
        { 0, 0, VK_FORMAT_R32G32B32_SFLOAT, 0 },
        { 1, 1, VK_FORMAT_R32_UINT, offsetof(InstanceData, objectID) },
        { 2, 1, VK_FORMAT_R32G32B32A32_SFLOAT, offsetof(InstanceData, model) + sizeof(glm::vec4) * 0 },
        { 3, 1, VK_FORMAT_R32G32B32A32_SFLOAT, offsetof(InstanceData, model) + sizeof(glm::vec4) * 1 },
        { 4, 1, VK_FORMAT_R32G32B32A32_SFLOAT, offsetof(InstanceData, model) + sizeof(glm::vec4) * 2 },
        { 5, 1, VK_FORMAT_R32G32B32A32_SFLOAT, offsetof(InstanceData, model) + sizeof(glm::vec4) * 3 }
    };
    pipeline = GraphicsPipeline::create(device, pipelineLayout, shaders, renderPass, extent,
                                        VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST, VK_CULL_MODE_BACK_BIT,
                                        VK_FRONT_FACE_CLOCKWISE, bindingDescription, attributeDescription);
}