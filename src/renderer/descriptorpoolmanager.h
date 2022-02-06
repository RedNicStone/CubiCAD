//
// Created by nic on 09/01/2022.
//

#pragma once

#ifndef CUBICAD_DESCRIPTORPOOLMANAGER_H
#define CUBICAD_DESCRIPTORPOOLMANAGER_H

#include "vulkan/descriptorpool.h"
#include "vulkan/descriptorset.h"


class DescriptorPoolManager {
  public:
    struct PoolSizes {
        std::vector<std::pair<VkDescriptorType, float>>
            sizes =
            {{VK_DESCRIPTOR_TYPE_SAMPLER, .5f}, {VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, .5f},
             {VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE, .5f}, {VK_DESCRIPTOR_TYPE_STORAGE_IMAGE, .5f},
             {VK_DESCRIPTOR_TYPE_UNIFORM_TEXEL_BUFFER, .5f}, {VK_DESCRIPTOR_TYPE_STORAGE_TEXEL_BUFFER, .5f},
             {VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, 8.f}, {VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, 4.f},
             {VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC, .5f}, {VK_DESCRIPTOR_TYPE_STORAGE_BUFFER_DYNAMIC, .5f},
             {VK_DESCRIPTOR_TYPE_INPUT_ATTACHMENT, 0.5f}};
    };

  private:
    std::shared_ptr<Device> device;

    std::shared_ptr<DescriptorPool> currentPool{};
    PoolSizes descriptorSizes;
    std::vector<std::shared_ptr<DescriptorPool>> usedPools;
    std::vector<std::shared_ptr<DescriptorPool>> freePools;

    std::shared_ptr<DescriptorPool> grabPool();

  public:
    static std::shared_ptr<DescriptorPoolManager> create(std::shared_ptr<Device> pDevice);

    void resetPools();
    std::shared_ptr<DescriptorSet> allocate(std::shared_ptr<DescriptorSetLayout> layout);
};

#endif //CUBICAD_DESCRIPTORPOOLMANAGER_H
