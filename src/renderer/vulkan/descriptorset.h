//
// Created by nic on 23/08/2021.
//

#pragma once

#ifndef CUBICAD_DESCRIPTORSET_H
#define CUBICAD_DESCRIPTORSET_H

#include <vulkan/vulkan.h>

#include <stdexcept>
#include <vector>

#include "vulkanclass.h"
#include "device.h"
#include "descriptorlayout.h"
#include "descriptorpool.h"
#include "imageview.h"
#include "uniformbuffer.h"
#include "sampler.h"


class UniformBuffer;

class Buffer;

class ImageView;

class DescriptorSet : public VulkanClass<VkDescriptorSet> {
  private:
    std::shared_ptr<Device> device;
    std::shared_ptr<DescriptorSetLayout> setLayout;
    std::shared_ptr<DescriptorPool> pool;

  public:
    static std::shared_ptr<DescriptorSet> create(std::shared_ptr<DescriptorSetLayout> pLayout,
                                                 const std::shared_ptr<DescriptorPool> &pPool);

    static std::shared_ptr<DescriptorSet> create(std::shared_ptr<DescriptorSetLayout> pLayout,
                                                 const std::shared_ptr<DescriptorPool> &pPool,
                                                 VkResult &result);

    static std::vector<std::shared_ptr<DescriptorSet>> create(std::vector<std::shared_ptr<DescriptorSetLayout>> pLayout,
                                                              const std::shared_ptr<DescriptorPool> &pPool);

    void updateUniformBuffer(const std::shared_ptr<UniformBuffer> &buffer,
                             uint32_t binding,
                             VkDeviceSize offset = 0,
                             uint32_t arrayElement = 0);

    void updateUniformBuffer(std::vector<std::shared_ptr<UniformBuffer>> buffer,
                             std::vector<uint32_t> binding,
                             std::vector<VkDeviceSize> offset = {},
                             std::vector<uint32_t> arrayElement = {});

    void updateUniformBuffer(const std::shared_ptr<Buffer> &buffer,
                             uint32_t binding,
                             VkDeviceSize size,
                             VkDeviceSize offset = 0,
                             uint32_t arrayElement = 0);

    void updateImageSampler(const std::shared_ptr<Sampler> &sampler, uint32_t binding);

    void updateStorageBuffer(const std::shared_ptr<Buffer> &buffer,
                             uint32_t binding,
                             uint32_t arrayElement = 0,
                             VkDeviceSize offset = 0,
                             VkDeviceSize range = VK_WHOLE_SIZE);
    void updateImages(const std::vector<std::shared_ptr<ImageView>> &imageViews,
                      VkDescriptorType type,
                      uint32_t binding,
                      uint32_t arrayElement = 0);
    void updateImage(const std::shared_ptr<ImageView> &imageView, uint32_t binding, uint32_t arrayElement = 0);
    void updateImage(const std::shared_ptr<ImageView> &imageView,
                     VkDescriptorType type,
                     uint32_t binding,
                     uint32_t arrayElement = 0);

    std::shared_ptr<Device> getDevice() { return device; }

    std::shared_ptr<DescriptorPool> getPool() { return pool; }

    std::shared_ptr<DescriptorSetLayout> getLayout() { return setLayout; }

    ~DescriptorSet();
};

#endif //CUBICAD_DESCRIPTORSET_H
