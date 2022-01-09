//
// Created by nic on 02/01/2022.
//

#pragma once

#ifndef CUBICAD_MATERIALLIBRARY_H
#define CUBICAD_MATERIALLIBRARY_H

#include <vector>
#include <glm/glm.hpp>

#include "mastermaterial.h"
#include "material.h"
#include "vulkan/uniformbuffer.h"


template<typename Parameters>
class MaterialLibrary {
  private:
    std::shared_ptr<Device> device;
    std::shared_ptr<Queue> transferQueue;

    bool bufferOOD = false;  // is the material buffer out of date?
    uint32_t materialCount = 0;  // how many materials are registered
    std::shared_ptr<Buffer> materialBuffer;  // the material buffer

    std::shared_ptr<MasterMaterial<Parameters>> masterMaterial;
    std::vector<std::shared_ptr<Material<Parameters>>> materials;

  public:
    static std::shared_ptr<MaterialLibrary<Parameters>> create(const std::shared_ptr<Device>& pDevice,
                                                               const std::shared_ptr<Queue>& pTransferQueue,
                                                               const std::vector<std::shared_ptr<Queue>>&
                                                               accessingQueues,
                                                               size_t reservedMaterials = 255);

    std::shared_ptr<Material<Parameters>> registerShader(Parameters parameters);

    void pushParameters();
};

template<typename Parameters>
std::shared_ptr<MaterialLibrary<Parameters>> MaterialLibrary<Parameters>::create(const std::shared_ptr<Device>& pDevice,
                                                                                 const std::shared_ptr<Queue>& pTransferQueue,
                                                                                 const std::vector<std::shared_ptr<Queue>>&
                                                                                     accessingQueues,
                                                                                 size_t reservedMaterials) {
    auto library = std::make_shared<MaterialLibrary<Parameters>>();
    library->device = pDevice;
    library->transferQueue = pTransferQueue;
    library->materials = std::vector<std::shared_ptr<Material<Parameters>>>(reservedMaterials);

    std::vector<glm::uint32> accessingQueueFamilyIndexes{};
    accessingQueueFamilyIndexes.reserve(accessingQueues.size() + 1);
    accessingQueueFamilyIndexes.push_back(pTransferQueue->getQueueFamilyIndex());
    for (const auto& queue : accessingQueues) {
        accessingQueueFamilyIndexes.push_back(queue->getQueueFamilyIndex());
    }
    library->materialBuffer = Buffer::create(pDevice, sizeof(Parameters) * reservedMaterials,
                                             VMA_MEMORY_USAGE_GPU_ONLY, VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT,
    accessingQueueFamilyIndexes);

    return library;
}

template<typename Parameters>
std::shared_ptr<Material<Parameters>> MaterialLibrary<Parameters>::registerShader(Parameters parameters) {
    if (materialCount + 1 < materials.size()) {
        materials[materialCount + 1] = parameters;
    } else {
        materials.push_back(parameters);
        materials.resize(materials.capacity());

        materialBuffer = Buffer::create(device, sizeof(Parameters) * materials.size(),
                                        VMA_MEMORY_USAGE_GPU_ONLY, VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT,
                                        accessingQueueFamilyIndexes);
    }
    bufferOOD = true;
}

template<typename Parameters>
void MaterialLibrary<Parameters>::pushParameters() {
    if (bufferOOD) {
        std::vector<Parameters> data(materials.size());
        for (size_t i = 0; i < materials.size(); i++) {
            data[i] = materials[i].getParameters();
        }
        materialBuffer->transferDataMapped(data.data());
    }
}

typedef MaterialLibrary<PBRMaterialParameters> PBRMaterialLibrary;

#endif //CUBICAD_MATERIALLIBRARY_H
