//
// Created by nic on 02/01/2022.
//

#include "materiallibrary.h"

#include <utility>


std::shared_ptr<MaterialLibrary> MaterialLibrary::create(std::shared_ptr<Device> pDevice, std::shared_ptr<Queue>
    pTransferQueue, size_t reservedMaterials) {
    auto materialLibrary = std::make_shared<MaterialLibrary>();

    materialLibrary->device = std::move(pDevice);
    materialLibrary->transferQueue = std::move(pTransferQueue);
    materialLibrary->materialBufferData.resize(reservedMaterials);
    materialLibrary->materialBuffer = Buffer::create(device, reservedMaterials * sizeof(MaterialParameters),
                                                     VMA_MEMORY_USAGE_GPU_ONLY, 0,
                                                     VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT, {
                                                         transferQueue->getQueueFamilyIndex() });
}
