//
// Created by nic on 25/01/2022.
//

#pragma once

#ifndef CUBICAD_TEXTURE_H
#define CUBICAD_TEXTURE_H

#include <stb_image.h>
#include <filesystem>

#include "vulkan/imageview.h"
#include "materialparameters.h"
#include "texturelibrary.h"


struct TextureQualitySettings;

/// A texture object that loads images from storage and uploads them to the GPU
class Texture {
  private:
    std::shared_ptr<Image> image;
    std::shared_ptr<ImageView> imageView;

  public:
    /// Create and upload a new texture to the GPU
    /// \param pDevice Device handle where the texture are created
    /// \param renderQueue Queue that will access the texture
    /// \param transferPool Queue to transfer the texture with
    /// \param filename Filename from which to load images from
    /// \param textureSettings Texture quality settings
    /// \param format Texture format
    /// \return Handle to the texture object
    static std::shared_ptr<Texture> create(const std::shared_ptr<Device> &pDevice,
                                           const std::shared_ptr<Queue> &renderQueue,
                                           const std::shared_ptr<CommandPool> &transferPool,
                                           const std::string &filename,
                                           const TextureQualitySettings &textureSettings,
                                           VkFormat format);

    std::shared_ptr<Image> getImage() { return image; }

    std::shared_ptr<ImageView> getImageView() { return imageView; }

};

#endif //CUBICAD_TEXTURE_H
