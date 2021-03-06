//
// Created by nic on 02/01/2022.
//

#include "materiallibrary.h"

#include <utility>


std::shared_ptr<MaterialLibrary> MaterialLibrary::create(const std::shared_ptr<Device> &pDevice,
                                                         const std::shared_ptr<CommandPool> &pTransferPool,
                                                         const std::shared_ptr<DescriptorPoolManager> &pDescriptorPool,
                                                         const std::shared_ptr<TextureLibrary>& textureLibrary,
                                                         const std::vector<std::shared_ptr<Queue>> &accessingQueues,
                                                         const std::shared_ptr<RenderPass> &renderPass,
                                                         uint32_t colorBlendStates,
                                                         VkExtent2D extent) {
    auto library = std::make_shared<MaterialLibrary>();
    library->device = pDevice;
    library->transferPool = pTransferPool;
    library->descriptorPool = pDescriptorPool;
    library->textureLibrary = textureLibrary;
    library->renderPass = renderPass;
    library->colorBlendStates = colorBlendStates;
    library->extent = extent;

    VkPhysicalDeviceProperties properties{};
    vkGetPhysicalDeviceProperties(pDevice->getPhysicalDevice()->getHandle(), &properties);
    library->alignment = properties.limits.minUniformBufferOffsetAlignment;

    std::vector<glm::uint32> accessingQueueFamilyIndexes{};
    accessingQueueFamilyIndexes.reserve(accessingQueues.size() + 1);
    accessingQueueFamilyIndexes.push_back(pTransferPool->getQueueFamily()->getQueueFamilyIndex());
    for (const auto &queue: accessingQueues) {
        accessingQueueFamilyIndexes.push_back(queue->getQueueFamilyIndex());
    }

    library->materialBuffer =
        DynamicBuffer::create(pDevice,
                              accessingQueueFamilyIndexes,
                              VMA_MEMORY_USAGE_GPU_ONLY,
                              VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT);

    return library;
}

std::shared_ptr<Material> MaterialLibrary::createMaterial(const std::shared_ptr<MasterMaterial> &masterMaterial,
                                                          void *parameters,
                                                          std::vector<std::shared_ptr<Texture>> textures,
                                                          const std::string& name) {
    for (auto& texture : textures)
        if (texture == nullptr)
            texture = textureLibrary->getDefaultTexture();  // todo: can be unbound?
    auto material = Material::create(masterMaterial, textures, parameters, name);
    materials.push_back(material);
    material->updateParameterBufferOffset(bufferSize + bufferOODSize);
    bufferOODSize += material->getMasterMaterial()->getPropertySize()
        + (alignment - (material->getMasterMaterial()->getPropertySize() % alignment)) % alignment;

    return material;
}

void MaterialLibrary::pushParameters() {
    if (bufferOODSize > 0) {
        std::vector<char> parameterBuffer(bufferSize + bufferOODSize);
        size_t p{};
        for (const auto &material: materials) {
            parameterBuffer.insert(parameterBuffer.begin() + static_cast<long>(p),
                                   static_cast<char *>(material->getParameters()),
                                   static_cast<char *>(material->getParameters()) + material->getMasterMaterial()->getPropertySize());
            p += material->getMasterMaterial()->getPropertySize()
                + (alignment - (material->getMasterMaterial()->getPropertySize() % alignment)) % alignment;
        }

        materialBuffer->getBufferPreserveContents(bufferSize + bufferOODSize, transferPool)
            ->transferDataStaged(parameterBuffer.data() + bufferSize, transferPool, bufferOODSize, bufferSize);

        bufferSize += bufferOODSize;
        bufferOODSize = 0;

        if (prevBuffer != materialBuffer->getBuffer()) {
            for (const auto &material: materials)
                material->updateParameterBuffer(materialBuffer->getBuffer());
            prevBuffer = materialBuffer->getBuffer();
        }
    }
}

std::shared_ptr<MasterMaterial> MaterialLibrary::createMasterMaterial(const std::shared_ptr<MasterMaterialTemplate> &masterMaterialTemplate,
                                                                      const std::string &pName,
                                                                      const std::shared_ptr<MaterialPropertyLayoutBuilt>
                                                                      &materialLayout) {
    std::shared_ptr<MaterialPropertyLayoutBuilt> materialLayout2;
    if (materialLayout == nullptr)
        materialLayout2 = masterMaterialTemplate->getPropertyLayout();
    else
        materialLayout2 = materialLayout;

    auto material =  MasterMaterial::create(device, masterMaterialTemplate->getShaders(), colorBlendStates, extent,
                                            materialLayout2,
                                  renderPass, descriptorPool, pName);
    material->updateImageSampler(textureLibrary);

    return material;
}

void MaterialLibrary::updateImageSampler() {
    std::unordered_set<std::shared_ptr<MasterMaterial>> masterMaterials{};
    for (const auto& material : materials)
        masterMaterials.insert(material->getMasterMaterial());

    for (const auto& masterMaterial : masterMaterials)
        masterMaterial->updateImageSampler(textureLibrary);
}
