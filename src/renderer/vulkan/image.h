//
// Created by nic on 16/07/2021.
//

#pragma once

#ifndef CUBICAD_IMAGE_H
#define CUBICAD_IMAGE_H

#include <vulkan/vulkan_core.h>
#include <vk_mem_alloc.h>

#include "device.h"
#include "vulkanclass.h"
#include "queues.h"
#include "imageview.h"

#include <vector>


class ImageView;

class Image : public VulkanClass<VkImage>, public std::enable_shared_from_this<Image> {
  private:
    std::shared_ptr<Device> device;
    VmaAllocation allocation;
    VmaAllocationInfo allocationInfo{};

    VkExtent3D extent;
    VkImageType type;
    VkFormat format;
    uint32_t mipLevels;
    uint32_t arrayLayers;

  public:
    static std::shared_ptr<Image> create(const std::shared_ptr<Device>& pDevice,
                                         VmaMemoryUsage memoryUsage,
                                         VkMemoryPropertyFlags preferredFlags,
                                         VkMemoryPropertyFlags requiredFlags,
                                         VkImageCreateInfo &createInfo,
                                         std::vector<uint32_t> &accessingQueues);

    static std::shared_ptr<Image> create(const std::shared_ptr<Device>& pDevice,
                                         const VkExtent2D &size,
                                         uint32_t mip_level,
                                         VkFormat format,
                                         VkImageUsageFlags usage,
                                         std::vector<uint32_t> &accessingQueues);

    std::shared_ptr<ImageView> createImageView(VkImageViewType viewType, VkImageSubresourceRange subresourceRange);

    std::shared_ptr<Device> getDevice() { return device; }

    [[nodiscard]] const VkExtent3D &getExtent() const { return extent; }

    [[nodiscard]] VkImageType getType() const { return type; }

    [[nodiscard]] VkFormat getFormat() const { return format; }

    [[nodiscard]] uint32_t getMipLevels() const { return mipLevels; }

    [[nodiscard]] uint32_t getArrayLayers() const { return arrayLayers; }

    void* map();
    void unmap();

    ~Image();
};

#endif //CUBICAD_IMAGE_H
