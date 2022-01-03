//
// Created by nic on 02/01/2022.
//

#pragma once

#ifndef CUBICAD_MATERIALLIBRARY_H
#define CUBICAD_MATERIALLIBRARY_H

#include <vector>
#include <glm/glm.hpp>

#include "mastermaterial.h"
#include "vulkan/uniformbuffer.h"


struct PBRMaterialParameters {
    std::pair<glm::uvec3, std::nullopt_t>   diffuse;
    std::pair<glm::uvec3, std::nullopt_t>   emission;
    std::pair<glm::uvec3, std::nullopt_t>   normal;
    std::pair<glm::float32, std::nullopt_t> transparency;
    std::pair<glm::float32, std::nullopt_t> roughness;
    std::pair<glm::float32, std::nullopt_t> metallic;
    std::pair<glm::float32, std::nullopt_t> specular;
    std::pair<glm::float32, std::nullopt_t> sheen;
    std::pair<glm::float32, std::nullopt_t> clearCoatThickness;
    std::pair<glm::float32, std::nullopt_t> clearCoatRoughness;
    std::pair<glm::float32, std::nullopt_t> anisotropy;
    std::pair<glm::float32, std::nullopt_t> anisotropyRotation;
};

class MaterialLibrary {
  private:
    typedef PBRMaterialParameters MaterialParameters;

    std::shared_ptr<Device> device;
    std::shared_ptr<Queue> transferQueue;

    uint32_t reservedCount;
    uint32_t materialCount;
    std::vector<MaterialParameters> materialBufferData;
    std::shared_ptr<Buffer> materialBuffer;

  public:
    std::shared_ptr<MaterialLibrary> create(std::shared_ptr<Device> pDevice, std::shared_ptr<Queue> pTransferQueue,
                                            size_t reservedMaterials = 255);
};

#endif //CUBICAD_MATERIALLIBRARY_H
