//
// Created by nic on 25/01/2022.
//

#pragma once

#ifndef CUBICAD_TEXTURELIBRARY_H
#define CUBICAD_TEXTURELIBRARY_H

#include "vulkan/sampler.h"
#include "texture.h"


class Texture;

struct TextureQualitySettings {
    float anisotropy;
    uint32_t mipLevels;
};

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
    static std::shared_ptr<TextureLibrary> create(const std::shared_ptr<Device> &pDevice,
                                                  const std::shared_ptr<Queue> &renderQueue,
                                                  const std::shared_ptr<CommandPool> &transferPool,
                                                  const TextureQualitySettings &settings);

    std::shared_ptr<Texture> createTexture(const std::string &filename, VkFormat format);

    std::shared_ptr<Device> getDevice() { return device; }
    std::shared_ptr<Sampler> getSampler() { return imageSampler; }
    std::shared_ptr<Texture> getDefaultTexture() { return defaultTexture; }
};

#endif //CUBICAD_TEXTURELIBRARY_H
