//
// Created by nic on 02/01/2022.
//

#include "materiallibrary.h"

#include <utility>


std::shared_ptr<MaterialLibrary> MaterialLibrary::create(const std::shared_ptr<Device>& pDevice,
                                                         const std::shared_ptr<CommandPool>& pTransferPool,
                                                         const std::shared_ptr<DescriptorPoolManager>& pDescriptorPool,
                                                         const std::vector<std::shared_ptr<Queue>>&
                                                         accessingQueues) {
    auto library = std::make_shared<MaterialLibrary>();
    library->device = pDevice;
    library->transferPool = pTransferPool;
    library->descriptorPool = pDescriptorPool;

    std::vector<glm::uint32> accessingQueueFamilyIndexes{};
    accessingQueueFamilyIndexes.reserve(accessingQueues.size() + 1);
    accessingQueueFamilyIndexes.push_back(pTransferPool->getQueueFamily()->getQueueFamilyIndex());
    for (const auto& queue : accessingQueues) {
        accessingQueueFamilyIndexes.push_back(queue->getQueueFamilyIndex());
    }

    library->materialBuffer = DynamicBuffer::create(pDevice, accessingQueueFamilyIndexes, VMA_MEMORY_USAGE_GPU_ONLY,
                                                    VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT |
                                                    VK_BUFFER_USAGE_TRANSFER_DST_BIT);

    return library;
}

std::shared_ptr<Material> MaterialLibrary::registerShader(const std::shared_ptr<Material>& material) {
    materials[material->getMasterMaterial()].push_back(material);
    material->updateParameterBufferOffset(bufferSize + bufferOODSize);
    bufferOODSize += material->getMasterMaterial()->getParameterSize();
}

void MaterialLibrary::pushParameters() {
    if (bufferOODSize > 0) {
        std::vector<char> parameterBuffer(bufferSize + bufferOODSize);
        for (const auto& pair : materials)
            for (const auto& material : pair.second)
                parameterBuffer.insert(parameterBuffer.end(), static_cast<char*>(material->getParameters()),
                                       static_cast<char*>(material->getParameters()) + pair.first->getParameterSize());

        auto prevBuffer = materialBuffer->getBuffer();
        materialBuffer->getBufferPreserveContents(bufferSize + bufferOODSize, transferPool)
                      ->transferDataStaged(parameterBuffer.data() + bufferSize, transferPool, bufferOODSize, bufferSize);

        bufferSize += bufferOODSize;
        bufferOODSize = 0;

        if (prevBuffer != materialBuffer->getBuffer())
            for (const auto& pair : materials)
                for (const auto& material : pair.second)
                    material->updateParameterBuffer(materialBuffer->getBuffer());
    }
}