//
// Created by nic on 23/08/2021.
//

#include "descriptorpool.h"

#include <utility>


std::shared_ptr<DescriptorPool> DescriptorPool::create(std::shared_ptr<Device> pDevice,
                                                       std::vector<VkDescriptorPoolSize> &sizes,
                                                       uint32_t maxSets) {
    auto descriptorPool = std::make_shared<DescriptorPool>();
    descriptorPool->device = std::move(pDevice);

    VkDescriptorPoolCreateInfo createInfo{};
    createInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
    createInfo.poolSizeCount = static_cast<uint32_t>(sizes.size());
    createInfo.pPoolSizes = sizes.data();
    createInfo.flags = VK_DESCRIPTOR_POOL_CREATE_FREE_DESCRIPTOR_SET_BIT;

    if (maxSets == 0) {
        for (const auto &size: sizes) {
            maxSets = std::max(size.descriptorCount, maxSets);
        }
    }

    createInfo.maxSets = maxSets;

    vkCreateDescriptorPool(descriptorPool->device->getHandle(), &createInfo, nullptr, &descriptorPool->handle);

    return descriptorPool;
}

DescriptorPool::~DescriptorPool() {
    vkDestroyDescriptorPool(device->getHandle(), handle, nullptr);
}
