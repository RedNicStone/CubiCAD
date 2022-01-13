//
// Created by nic on 02/01/2022.
//

#include "materiallibrary.h"

#include <utility>


std::shared_ptr<MaterialLibrary> MaterialLibrary::create(const std::shared_ptr<Device>& pDevice,
                                                         const std::shared_ptr<Queue>& pTransferQueue,
                                                         const std::vector<std::shared_ptr<Queue>>&
                                                         accessingQueues,
                                                         size_t reservedMaterials) {
    auto library = std::make_shared<MaterialLibrary>();
    library->device = pDevice;
    library->transferQueue = pTransferQueue;
    library->materials = std::vector<std::shared_ptr<Material>>(reservedMaterials);

    library->accessingQueueFamilyIndexes.reserve(accessingQueues.size() + 1);
    library->accessingQueueFamilyIndexes.push_back(pTransferQueue->getQueueFamilyIndex());
    for (const auto& queue : accessingQueues) {
        library->accessingQueueFamilyIndexes.push_back(queue->getQueueFamilyIndex());
    }
    library->materialBuffer = Buffer::create(pDevice, sizeof(PBRMaterialParameters) * reservedMaterials,
                                             VMA_MEMORY_USAGE_GPU_ONLY, VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT,
                                             library->accessingQueueFamilyIndexes);

    return library;
}

std::shared_ptr<Material> MaterialLibrary::registerShader(PBRMaterialParameters parameters) {
    if (materialCount + 1 < materials.size()) {
        //materials[materialCount + 1] = parameters;
    } else {
        //materials.push_back(parameters);
        materials.resize(materials.capacity());

        materialBuffer = Buffer::create(device, sizeof(PBRMaterialParameters) * materials.size(),
                                        VMA_MEMORY_USAGE_GPU_ONLY, VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT,
                                        accessingQueueFamilyIndexes);
    }
    bufferOOD = true;
}

void MaterialLibrary::pushParameters() {
    if (bufferOOD) {
        std::vector<PBRMaterialParameters> data(materials.size());
        for (size_t i = 0; i < materials.size(); i++) {
            //data[i] = materials[i].getParameters();
        }
        //materialBuffer->transferDataStaged(data.data());
    }
}
