//
// Created by nic on 31/12/2021.
//

#include "mastermaterial.h"


std::shared_ptr<MasterMaterial> MasterMaterial::create(const std::shared_ptr<Device>& pDevice, const
std::vector<std::shared_ptr<GraphicsShader>>& vShaders, uint32_t vColorBlendStates, VkExtent2D vExtent, const
MaterialPropertyLayoutBuilt& layout, const
std::shared_ptr<RenderPass>&
pRenderPass, const std::shared_ptr<DescriptorPoolManager>& descriptorManager, const std::string& pName) {
    auto material = std::make_shared<MasterMaterial>();
    material->device = pDevice;
    material->extent = vExtent;
    material->shaders = vShaders;
    material->renderPass = pRenderPass;
    material->colorBlendStates = vColorBlendStates;
    material->propertyLayout = layout;

    if (pName.empty())
        material->name = "UnnamedMaterial";
    else
        material->name = pName;

    material->generateMaterialSetLayout();
    material->masterMaterialSet = descriptorManager->allocate(material->masterMaterialSetLayout);
    return material;
}

void MasterMaterial::generateMaterialSetLayout() {
    std::vector<VkDescriptorSetLayoutBinding> layoutBindings(2);
    layoutBindings[0].binding = 0;
    layoutBindings[0].pImmutableSamplers = nullptr;
    layoutBindings[0].stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;
    layoutBindings[0].descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
    layoutBindings[0].descriptorCount = 1;

    layoutBindings[1].binding = 1;
    layoutBindings[1].pImmutableSamplers = nullptr;
    layoutBindings[1].stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;
    layoutBindings[1].descriptorType = VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE;

    uint32_t imageCount = 0;
    for (const auto& property : propertyLayout.properties)
        if (property->input & MATERIAL_PROPERTY_INPUT_TEXTURE)
            imageCount++;

    layoutBindings[1].descriptorCount = imageCount;
    
    std::vector<VkDescriptorSetLayoutBinding> masterLayoutBindings(1);
    masterLayoutBindings[0].binding = 0;
    masterLayoutBindings[0].pImmutableSamplers = nullptr;
    masterLayoutBindings[0].stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;
    masterLayoutBindings[0].descriptorType = VK_DESCRIPTOR_TYPE_SAMPLER;
    masterLayoutBindings[0].descriptorCount = 1;

    materialSetLayout = DescriptorSetLayout::create(device, layoutBindings);
    masterMaterialSetLayout = DescriptorSetLayout::create(device, masterLayoutBindings);
}

void MasterMaterial::updateDescriptorSetLayouts(const std::shared_ptr<DescriptorSetLayout>& sceneLayout,
                                                bool enableDepthStencil) {
    std::vector<std::shared_ptr<DescriptorSetLayout>> descriptorLayouts{ sceneLayout, masterMaterialSetLayout,
                                                                         materialSetLayout };

    pipelineLayout = PipelineLayout::create(device, descriptorLayouts);

    std::vector<VkVertexInputBindingDescription> bindingDescription {
        { 0, sizeof(Vertex), VK_VERTEX_INPUT_RATE_VERTEX },
        { 1, sizeof(InstanceData), VK_VERTEX_INPUT_RATE_INSTANCE }
    };
    std::vector<VkVertexInputAttributeDescription> attributeDescription {
        { 0, 0, VK_FORMAT_R32G32B32_SFLOAT, offsetof(Vertex, pos) },
        { 1, 0, VK_FORMAT_R16G16_UNORM, offsetof(Vertex, uv) },
        { 2, 1, VK_FORMAT_R32_UINT, offsetof(InstanceData, objectID) },
        { 3, 1, VK_FORMAT_R32G32B32A32_SFLOAT, offsetof(InstanceData, model) + sizeof(glm::vec4) * 0 },
        { 4, 1, VK_FORMAT_R32G32B32A32_SFLOAT, offsetof(InstanceData, model) + sizeof(glm::vec4) * 1 },
        { 5, 1, VK_FORMAT_R32G32B32A32_SFLOAT, offsetof(InstanceData, model) + sizeof(glm::vec4) * 2 },
        { 6, 1, VK_FORMAT_R32G32B32A32_SFLOAT, offsetof(InstanceData, model) + sizeof(glm::vec4) * 3 }
    };
    pipeline = GraphicsPipeline::create(device, pipelineLayout, shaders, renderPass, colorBlendStates, extent,
                                        VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST, VK_CULL_MODE_NONE,
                                        VK_FRONT_FACE_CLOCKWISE, bindingDescription, attributeDescription, enableDepthStencil);
}

void MasterMaterial::updateImageSampler(const std::shared_ptr<TextureLibrary> &textureLibrary) {
    masterMaterialSet->updateImageSampler(textureLibrary->getSampler(), 0);
}

std::string MasterMaterial::generateMaterialName() {
    subMaterials++;
    return name + " [" + std::to_string(subMaterials) + "]";
}
