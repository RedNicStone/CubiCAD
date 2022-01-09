//
// Created by nic on 30/12/2021.
//

#pragma once

#ifndef CUBICAD_MATERIAL_H
#define CUBICAD_MATERIAL_H

#include "materiallibrary.h"
#include "vulkan/graphicspipeline.h"


struct MaterialParameters {
    glm::uint32 materialIndex;
};

struct PBRMaterialParameters : public MaterialParameters {
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

template<typename Parameters>
class Material {
  private:
    std::shared_ptr<MasterMaterial<Parameters>> masterMaterial;
    Parameters parameters;

    std::shared_ptr<GraphicsPipeline> pipeline;

  public:
    Parameters getParameters() { return parameters; }
};

typedef Material<PBRMaterialParameters> PBRMaterial;

#endif //CUBICAD_MATERIAL_H
