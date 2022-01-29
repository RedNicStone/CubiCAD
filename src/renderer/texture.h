//
// Created by nic on 25/01/2022.
//

#pragma once

#ifndef CUBICAD_TEXTURE_H
#define CUBICAD_TEXTURE_H

#include <stb_image.h>

#include "vulkan/imageview.h"
#include "materialparameters.h"
#include "texturelibrary.h"


struct TextureQualitySettings;

class Texture {
  private:
    std::shared_ptr<Image> image;
    std::shared_ptr<ImageView> imageView;

  public:
    static std::shared_ptr<Texture> create(const std::shared_ptr<Device>& pDevice,
                                           const std::shared_ptr<Queue>& renderQueue,
                                           const std::shared_ptr<CommandPool>& transferPool, const std::string& filename,
                                           const TextureQualitySettings& textureSettings, VkFormat format);

    std::shared_ptr<Image> getImage() { return image; }
    std::shared_ptr<ImageView> getImageView() { return imageView; }

};

#endif //CUBICAD_TEXTURE_H
