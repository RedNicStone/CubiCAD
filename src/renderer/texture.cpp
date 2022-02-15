//
// Created by nic on 25/01/2022.
//

#define STB_IMAGE_IMPLEMENTATION

#include "texture.h"


std::shared_ptr<Texture> Texture::create(const std::shared_ptr<Device> &pDevice,
                                         const std::shared_ptr<Queue> &renderQueue,
                                         const std::shared_ptr<CommandPool> &transferPool,
                                         const std::string &filename,
                                         const TextureQualitySettings &textureSettings,
                                         VkFormat format) {
    auto texture = std::make_shared<Texture>();

    if (!std::filesystem::exists(filename))
        throw std::runtime_error("Could not open texture file at location: " + filename);

    int texWidth, texHeight, texChannels;
    stbi_uc *data = stbi_load(filename.c_str(), &texWidth, &texHeight, &texChannels, STBI_rgb_alpha);

    if (!data)
        throw std::runtime_error("Texture file invalid at location: " + filename);

    std::vector<uint32_t> accessingQueues{renderQueue->getQueueFamilyIndex()};

    texture->image =
        Image::create(pDevice,
                      {static_cast<uint32_t>(texWidth), static_cast<uint32_t>(texHeight)},
                      textureSettings.mipLevels,
                      format,
                      VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT,
                      accessingQueues);
    texture->image->transferDataStaged(data, transferPool, static_cast<VkDeviceSize>(
        static_cast<uint>(texWidth * texHeight * 4)));
    delete[] data;

    texture->imageView =
        texture->image
            ->createImageView(VK_IMAGE_VIEW_TYPE_2D, {VK_IMAGE_ASPECT_COLOR_BIT, 0, textureSettings.mipLevels, 0, 1});

    return texture;
}
