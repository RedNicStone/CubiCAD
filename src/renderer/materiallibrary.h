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
#include "descriptorpoolmanager.h"
#include "dynamicbuffer.h"
#include "vulkan/sampler.h"
#include "mastermaterialtemplate.h"


class MasterMaterial;

class Material;

class Texture;

class TextureLibrary;

/// Material library storing and maintaining materials
class MaterialLibrary {
  private:
    std::shared_ptr<Device> device;
    std::shared_ptr<CommandPool> transferPool;

    size_t alignment;
    uint32_t bufferOODSize{};  // is the material buffer out of date?
    uint32_t bufferSize;  // how many materials are registered
    std::shared_ptr<DynamicBuffer> materialBuffer;  // the material buffer
    std::shared_ptr<Buffer> prevBuffer{};

    std::shared_ptr<DescriptorPoolManager> descriptorPool;
    std::shared_ptr<TextureLibrary> textureLibrary;

    std::vector<std::shared_ptr<Material>> materials;

    std::shared_ptr<RenderPass> renderPass;
    uint32_t colorBlendStates;
    VkExtent2D extent;

  public:
    /// Create a new material library
    /// \param pDevice Device to store the materials on
    /// \param pTransferPool Transfer pool for carrying out memory transfers
    /// \param pDescriptorPool Descriptor pool for allocating descriptor sets
    /// \param textureLibrary Texture library for getting default textures
    /// \param accessingQueues Queues accessing the materials
    /// \param renderPass Render pass the materials are used in
    /// \param colorBlendStates Number of blend states in the render pass
    /// \param extent Surface extend
    /// \return Handle to a material library
    static std::shared_ptr<MaterialLibrary> create(const std::shared_ptr<Device> &pDevice,
                                                   const std::shared_ptr<CommandPool> &pTransferPool,
                                                   const std::shared_ptr<DescriptorPoolManager> &pDescriptorPool,
                                                   const std::shared_ptr<TextureLibrary>& textureLibrary,
                                                   const std::vector<std::shared_ptr<Queue>> &accessingQueues,
                                                   const std::shared_ptr<RenderPass> &renderPass,
                                                   uint32_t colorBlendStates,
                                                   VkExtent2D extent);

    /// Create new master material
    /// \param masterMaterialTemplate Master template for material creation
    /// \param pName Name of the material
    /// \param materialLayout Input layout of material
    /// \return Handle to new master material
    std::shared_ptr<MasterMaterial> createMasterMaterial(const std::shared_ptr<MasterMaterialTemplate>
                                                         &masterMaterialTemplate,
                                                         const std::string &pName,
                                                         const std::shared_ptr<MaterialPropertyLayoutBuilt>
                                                         &materialLayout = nullptr);

    /// Create new material
    /// \param masterMaterial Master material to use
    /// \param parameters Input parameters
    /// \param textures Input textures
    /// \param name Name of the material
    /// \return Handle to the new material
    std::shared_ptr<Material> createMaterial(const std::shared_ptr<MasterMaterial> &masterMaterial,
                                             void *parameters,
                                             std::vector<std::shared_ptr<Texture>> textures = {},
                                             const std::string& name = "");

    /// Update all image samplers
    void updateImageSampler();

    /// Push all material parameters to the GPU
    void pushParameters();
};

#endif //CUBICAD_MATERIALLIBRARY_H
