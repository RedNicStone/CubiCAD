//
// Created by nic on 25/01/2022.
//

#include "texturelibrary.h"


std::shared_ptr<TextureLibrary> TextureLibrary::create(const std::shared_ptr<Device> &pDevice,
                                                       const std::shared_ptr<Queue> &renderQueue,
                                                       const std::shared_ptr<CommandPool> &transferPool,
                                                       const TextureQualitySettings &settings) {
    auto textureLibrary = std::make_shared<TextureLibrary>();
    textureLibrary->imageSampler = Sampler::create(pDevice, settings.anisotropy);
    textureLibrary->renderQueue = renderQueue;
    textureLibrary->transferPool = transferPool;
    textureLibrary->device = pDevice;
    textureLibrary->settings = settings;

    textureLibrary->defaultTexture = textureLibrary->createTexture("resources/textures/default_4096x4096.png",
                                                                  VK_FORMAT_R8G8B8A8_UNORM);

    return textureLibrary;
}

std::shared_ptr<Texture> TextureLibrary::createTexture(const std::string &filename, VkFormat format) {
    if (textures[filename] == nullptr) {
        auto tex = Texture::create(device, renderQueue, transferPool, filename, settings, format);
        textures[filename] = tex;
        return tex;
    }
    return textures[filename];
}
