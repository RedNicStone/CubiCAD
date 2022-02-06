//
// Created by nic on 25/01/2022.
//

#include "sampler.h"


std::shared_ptr<Sampler> Sampler::create(const std::shared_ptr<Device> &pDevice, float anisotropy) {
    auto sampler = std::make_shared<Sampler>();
    sampler->device = pDevice;

    VkSamplerCreateInfo createInfo{};
    createInfo.sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;
    createInfo.minFilter = VK_FILTER_LINEAR;
    createInfo.magFilter = VK_FILTER_LINEAR;
    createInfo.addressModeU = VK_SAMPLER_ADDRESS_MODE_REPEAT;
    createInfo.addressModeV = VK_SAMPLER_ADDRESS_MODE_REPEAT;
    createInfo.addressModeW = VK_SAMPLER_ADDRESS_MODE_REPEAT;
    createInfo.anisotropyEnable = (anisotropy > 0.0f);
    createInfo.maxAnisotropy = anisotropy;
    createInfo.borderColor = VK_BORDER_COLOR_FLOAT_OPAQUE_WHITE;
    createInfo.unnormalizedCoordinates = VK_FALSE;
    createInfo.compareEnable = VK_FALSE;
    createInfo.compareOp = VK_COMPARE_OP_ALWAYS;
    createInfo.mipmapMode = VK_SAMPLER_MIPMAP_MODE_LINEAR;
    createInfo.mipLodBias = 0.0f;
    createInfo.minLod = 0.0f;
    createInfo.maxLod = 0.0f;

    if (vkCreateSampler(pDevice->getHandle(), &createInfo, nullptr, &sampler->handle) != VK_SUCCESS) {
        throw std::runtime_error("could not create sampler!");
    }

    return sampler;
}

Sampler::~Sampler() {
    vkDestroySampler(device->getHandle(), handle, nullptr);
}
