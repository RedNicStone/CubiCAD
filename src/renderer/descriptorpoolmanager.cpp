//
// Created by nic on 09/01/2022.
//

#include "descriptorpoolmanager.h"

#include <utility>

std::shared_ptr<DescriptorPoolManager> DescriptorPoolManager::create(std::shared_ptr<Device> pDevice) {
    auto poolManager = std::make_shared<DescriptorPoolManager>();
    poolManager->device = std::move(pDevice);

    return poolManager;
}

std::shared_ptr<DescriptorSet> DescriptorPoolManager::allocate(std::shared_ptr<DescriptorSetLayout> layout) {
    if (currentPool) {
        currentPool = grabPool();
        usedPools.push_back(currentPool);
    }

    std::shared_ptr<DescriptorSet> set;
    VkResult result;
    try {
        set = DescriptorSet::create(std::move(layout), currentPool);
    } catch (std::runtime_error& e) {
        if (result == VK_ERROR_OUT_OF_POOL_MEMORY) {
            //allocate a new pool and retry
            currentPool = grabPool();
            usedPools.push_back(currentPool);

            set = DescriptorSet::create(std::move(layout), currentPool);
        } else if (result == VK_SUCCESS) {
            return set;
        } else {
            throw std::runtime_error("could not create descriptor set!");
        }
    }
}

std::shared_ptr<DescriptorPool> DescriptorPoolManager::grabPool() {
    if (!freePools.empty()) {
        auto pool = freePools.back();
        freePools.pop_back();
        return pool;
    }
    else {
        std::vector<VkDescriptorPoolSize> sizes;
        sizes.reserve(descriptorSizes.sizes.size());
        for (auto sz : descriptorSizes.sizes) {
            sizes.push_back({ sz.first, uint32_t(sz.second * 1000) });
        }
        return DescriptorPool::create(device, sizes, 1000);
    }
}

void DescriptorPoolManager::resetPools() {
    for (const auto& pool : usedPools) {
        pool->reset();
    }

    freePools = usedPools;
    usedPools.clear();
    currentPool = nullptr;
}
