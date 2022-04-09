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

/// Material class that can be used in a mesh for drawing
class Material {
  private:
    std::shared_ptr<MasterMaterial> masterMaterial;
    std::shared_ptr<DescriptorSet> materialSet;

    void *parameters;
    size_t parameterBufferOffset;
    std::string name;

  public:
    /// Create a new material
    /// \param pMasterMaterial Master material to use
    /// \param textures Texture inputs to use
    /// \param parameters Material input parameters to use
    /// \param pName Material name
    /// \return Valid handle to the new material
    static std::shared_ptr<Material> create(const std::shared_ptr<MasterMaterial> &pMasterMaterial,
                                            const std::vector<std::shared_ptr<Texture>> &textures,
                                            void *parameters,
                                            const std::string &pName = "");

    /// Update the parameter buffer with material information
    /// \param buffer Buffer to update
    void updateParameterBuffer(const std::shared_ptr<Buffer> &buffer);

    /// Update the offset in the material parameter buffer
    /// \param offset Offset to use
    void updateParameterBufferOffset(size_t offset) { parameterBufferOffset = offset; }
    void setName(const std::string &pName) { name = pName; }

    std::string getName() { return name; }
    void *getParameters() { return parameters; }
    std::shared_ptr<MasterMaterial> getMasterMaterial() { return masterMaterial; }
    std::shared_ptr<DescriptorSet> getDescriptorSet() { return materialSet; }

    ~Material();
};

#endif //CUBICAD_MATERIAL_H
