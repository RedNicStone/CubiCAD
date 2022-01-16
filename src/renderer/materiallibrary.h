//
// Created by nic on 02/01/2022.
//

#pragma once

#ifndef CUBICAD_MATERIALLIBRARY_H
#define CUBICAD_MATERIALLIBRARY_H

#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEFAULT_ALIGNED_GENTYPES

#include <vector>
#include <glm/glm.hpp>

#include "mastermaterial.h"
#include "material.h"
#include "vulkan/uniformbuffer.h"


class MasterMaterial;

class Material;

struct PBRMaterialParameters;

class MaterialLibrary {
  private:
    std::shared_ptr<Device> device;
    std::shared_ptr<Queue> transferQueue;
    std::vector<glm::uint32> accessingQueueFamilyIndexes{};

    bool bufferOOD = false;  // is the material buffer out of date?
    uint32_t materialCount{};  // how many materials are registered
    std::shared_ptr<Buffer> materialBuffer;  // the material buffer

    std::shared_ptr<MasterMaterial> masterMaterial;
    std::vector<std::shared_ptr<Material>> materials;

  public:
    static std::shared_ptr<MaterialLibrary> create(const std::shared_ptr<Device>& pDevice,
                                                               const std::shared_ptr<Queue>& pTransferQueue,
                                                               const std::vector<std::shared_ptr<Queue>>&
                                                               accessingQueues,
                                                               size_t reservedMaterials = 256);

    std::shared_ptr<Material> registerShader(PBRMaterialParameters parameters);

    void pushParameters();
};

#endif //CUBICAD_MATERIALLIBRARY_H
