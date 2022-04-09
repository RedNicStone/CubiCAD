//
// Created by nic on 25/01/2022.
//

#pragma once

#ifndef CUBICAD_TEXTURELIBRARY_H
#define CUBICAD_TEXTURELIBRARY_H

#include "vulkan/sampler.h"
#include "texture.h"


class Texture;

/// Struct for storing texture settings
struct TextureQualitySettings {
    float anisotropy;   //< Anisotropy filtering factor
    uint32_t mipLevels; //< Number of MIP levels
};

/// Library that manages texture objects
class TextureLibrary {
  private:
    std::shared_ptr<Device> device;
    std::shared_ptr<Queue> renderQueue;
    std::shared_ptr<CommandPool> transferPool;

    std::shared_ptr<Sampler> imageSampler;
    TextureQualitySettings settings;

    std::shared_ptr<Texture> defaultTexture;

    std::unordered_map<std::string, std::shared_ptr<Texture>> textures{};

  public:
    /// Create a new texture library
    /// \param pDevice Device to allocate textures on
    /// \param renderQueue Render queue from which the textures will be accessed
    /// \param transferPool Transfer pool to use for transferring the image data
    /// \param settings Texture quality settings for all textures
    /// \return Valid handle to texture library object
    static std::shared_ptr<TextureLibrary> create(const std::shared_ptr<Device> &pDevice,
                                                  const std::shared_ptr<Queue> &renderQueue,
                                                  const std::shared_ptr<CommandPool> &transferPool,
                                                  const TextureQualitySettings &settings);

    /// Find or create texture given the filename and format
    /// \param filename Filename from which to load the texture from
    /// \param format Format for the texture image
    /// \return Handle to texture object
    std::shared_ptr<Texture> createTexture(const std::string &filename, VkFormat format);
    /// Update texture settings
    /// \param textureSettings Settings to apply
    void updateSettings(const TextureQualitySettings &textureSettings);

    std::shared_ptr<Device> getDevice() { return device; }
    std::shared_ptr<Sampler> getSampler() { return imageSampler; }
    std::shared_ptr<Texture> getDefaultTexture() { return defaultTexture; }
    TextureQualitySettings getTextureSettings() { return settings; }
};

#endif //CUBICAD_TEXTURELIBRARY_H
