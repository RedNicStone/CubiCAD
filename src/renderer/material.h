//
// Created by nic on 30/12/2021.
//

#pragma once

#ifndef CUBICAD_MATERIAL_H
#define CUBICAD_MATERIAL_H

#include "materiallibrary.h"
#include "mastermaterial.h"
#include "vulkan/graphicspipeline.h"
#include "texture.h"


class MasterMaterial;

class DescriptorPoolManager;

struct PBRMaterialParameters {
    alignas(4)  glm::uint32 materialIndex;
    alignas(16) glm::uvec3 diffuse;
    alignas(16) glm::uvec3 emission;
    alignas(16) glm::uvec3 normal;
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
    std::shared_ptr<DescriptorSet> materialSet;

    void *parameters;
    size_t parameterBufferOffset;
    std::string name;

  public:
    static std::shared_ptr<Material> create(const std::shared_ptr<MasterMaterial> &pMasterMaterial,
                                            const std::vector<std::shared_ptr<Texture>> &textures,
                                            void *parameters,
                                            const std::string &pName = "");

    void updateParameterBuffer(const std::shared_ptr<Buffer> &buffer);

    void updateParameterBufferOffset(size_t offset) { parameterBufferOffset = offset; }

    void setName(const std::string &pName) { name = pName; }

    std::string getName() { return name; }

    void *getParameters() { return parameters; }

    std::shared_ptr<MasterMaterial> getMasterMaterial() { return masterMaterial; }

    std::shared_ptr<DescriptorSet> getDescriptorSet() { return materialSet; }

    ~Material();
};

#endif //CUBICAD_MATERIAL_H
