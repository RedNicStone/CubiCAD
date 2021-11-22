//
// Created by nic on 23/08/2021.
//

#include "descriptorpool.h"


DescriptorPool::DescriptorPool(Device *pDevice, std::vector<VkDescriptorPoolSize> &sizes, uint32_t maxSets)
    : device(pDevice) {
    VkDescriptorPoolCreateInfo createInfo{};
    createInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
    createInfo.poolSizeCount = static_cast<uint32_t>(sizes.size());
    createInfo.pPoolSizes = sizes.data();

    if (maxSets == 0) {
        for (const auto &size : sizes) {
            maxSets = std::max(size.descriptorCount, maxSets);
        }
    }

    createInfo.maxSets = maxSets;

    vkCreateDescriptorPool(device->getHandle(), &createInfo, nullptr, &handle);
}

DescriptorPool::~DescriptorPool() {
    vkDestroyDescriptorPool(device->getHandle(), handle, nullptr);
}
