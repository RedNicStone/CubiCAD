//
// Created by nic on 30/12/2021.
//

#pragma once

#ifndef CUBICAD_MATERIAL_H
#define CUBICAD_MATERIAL_H

#include "materiallibrary.h"
#include "mastermaterial.h"
#include "vulkan/graphicspipeline.h"


class MasterMaterial;

struct PBRMaterialParameters {
    alignas(4)  glm::uint32 materialIndex;
    alignas(16) glm::uvec3   diffuse;
    alignas(16) glm::uvec3   emission;
    alignas(16) glm::uvec3   normal;
    alignas(4)  glm::float32 transparency;
    alignas(4)  glm::float32 roughness;
    alignas(4)  glm::float32 metallic;
    alignas(4)  glm::float32 specular;
    alignas(4)  glm::float32 sheen;
    alignas(4)  glm::float32 clearCoatThickness;
    alignas(4)  glm::float32 clearCoatRoughness;
    alignas(4)  glm::float32 anisotropy;
    alignas(4)  glm::float32 anisotropyRotation;
};

class Material {
  private:
    std::shared_ptr<MasterMaterial> masterMaterial;
    PBRMaterialParameters parameters;

    std::shared_ptr<DescriptorSet> materialSet;

  public:
    PBRMaterialParameters getParameters() { return parameters; }
    std::shared_ptr<MasterMaterial> getMasterMaterial() { return masterMaterial; }
    std::shared_ptr<DescriptorSet> getDescriptorSet() { return materialSet; }
};

#endif //CUBICAD_MATERIAL_H
